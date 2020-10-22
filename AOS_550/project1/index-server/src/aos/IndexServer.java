package aos;

import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadPoolExecutor;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.IOException;
import java.io.Serializable;

public class IndexServer {
	private static final int SERVER_PORT = 5056;
	public static HashMap<String, ArrayList<String>> filesMapObj = new HashMap<String, ArrayList<String>>();
	private ServerSocket serverSocket;

	public static HashMap<String, ArrayList<String>> getFilesMapObj() {
		return filesMapObj;
	}

	public static void main(String[] args) throws IOException {
		ServerSocket serverSocket = new ServerSocket(SERVER_PORT);
		ThreadPoolExecutor executor = (ThreadPoolExecutor) Executors.newCachedThreadPool();
		System.out.println("Server is running on " + SERVER_PORT);
		while (true) {
			Socket socket = null;
			try {
				// socket object to receive incoming client requests
				socket = serverSocket.accept();
				System.out.println("A new client is connected : " + socket);

				// obtaining input and out streams
				ObjectInputStream objInputStream = new ObjectInputStream(socket.getInputStream());
				ObjectOutputStream objOutputStream = new ObjectOutputStream(socket.getOutputStream());

				System.out.println("Assigning a thread for this client request");
				// create a new thread object
				ClientRequestHandler threadReq = new ClientRequestHandler(socket, objInputStream, objOutputStream);
				// Invoking the start() method
				executor.execute(threadReq);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

}

class ClientRequestHandler implements Runnable {
	private final Socket threadSocket;
	private final ObjectInputStream objInputStream;
	private final ObjectOutputStream objOutputStream;

	public ClientRequestHandler(Socket thrSocket, ObjectInputStream objInputStream,
			ObjectOutputStream objOutputStream) {
		this.threadSocket = thrSocket;
		this.objInputStream = objInputStream;
		this.objOutputStream = objOutputStream;
	}

	@Override
	public void run() {
		try {
			Object obj = this.objInputStream.readObject();
			if (obj instanceof PeerRegister) {
				PeerRegister peerInfo = (PeerRegister) obj;
				HashMap<String, ArrayList<String>> mapObj = IndexServer.getFilesMapObj();

				String peerIpAddr = this.threadSocket.getInetAddress().getHostName();
				System.out.println(peerIpAddr + " is registering all its files.");
				for (String peerFileName : peerInfo.getFileNames()) {
					ArrayList<String> ipAddresses = mapObj.get(peerFileName);
					if (ipAddresses == null) {
						ipAddresses = new ArrayList<String>();
						mapObj.put(peerFileName, ipAddresses);
					}
					if (ipAddresses.contains(peerIpAddr) == false) {
						ipAddresses.add(peerIpAddr);
					}
				}

				System.out
						.println("Registered Peer ->>  " + peerIpAddr + " with :" + peerInfo.getFileNames().size() + " files");
				this.objOutputStream.writeObject("Done");
			} else if (obj instanceof PeerFileLookup) {
				PeerFileLookup lookupObj = (PeerFileLookup) obj;
				String fileName = lookupObj.getFileName();
				System.out.println("Searching file in the Server Index" + fileName);
				ArrayList<String> retList = IndexServer.getFilesMapObj().get(fileName);
				System.out.println("Sending peers list to client " + String.valueOf(retList));
				this.objOutputStream.writeObject(retList);
				this.objOutputStream.flush();
			} else {
				System.out.println("Invalid request arguments received");
			}
		} catch (IOException | ClassNotFoundException e) {
			e.printStackTrace();
		} finally {
			try {
				this.objInputStream.close();
				this.objOutputStream.close();
				this.threadSocket.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
}

class PeerRegister implements Serializable {
	private static final long serialVersionUID = 7095249478609523764L;
	private ArrayList<String> fileNames;

	public ArrayList<String> getFileNames() {
		return fileNames;
	}

	public PeerRegister(ArrayList<String> fileNames) {
		this.fileNames = fileNames;
	}
}

class PeerFileLookup implements Serializable {
	private static final long serialVersionUID = 2528812348519341073L;
	private String fileName;

	public String getFileName() {
		return fileName;
	}

	public PeerFileLookup(String fileName) {
		this.fileName = fileName;
	}
}