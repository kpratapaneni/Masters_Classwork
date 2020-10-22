package aos;

import java.net.*;
import java.io.*;

public class IndexServerOld extends Thread {
   private ServerSocket indexServerSocket;
   
   public IndexServerOld(int port) throws IOException {
      indexServerSocket = new ServerSocket(port);
   }

   public void run() {
      while(true) {
         try {
            System.out.println("Waiting for client on port " + 
               indexServerSocket.getLocalPort() + "...");
            Socket server = indexServerSocket.accept();
            
            System.out.println("Just connected to " + server.getRemoteSocketAddress());
            DataInputStream in = new DataInputStream(server.getInputStream());
            
            System.out.println(in.readUTF());
            DataOutputStream out = new DataOutputStream(server.getOutputStream());
            out.writeUTF("Thank you for connecting to " + server.getLocalSocketAddress()
               + "\nGoodbye!");
            server.close();
            
         } catch (SocketTimeoutException s) {
            System.out.println("Socket timed out!");
            break;
         } catch (IOException e) {
            e.printStackTrace();
            break;
         }
      }
   }
   
   public static void main(String [] args) {
      try {
         Thread t = new IndexServerOld(5056);
         t.start();
      } catch (IOException e) {
         e.printStackTrace();
      }
   }
}


/*
 package aos;

import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.IOException;
import java.io.Serializable;

public class IndexServer {
	private static final int SERVER_PORT = 5056;
	public static HashMap<String, ArrayList<String>> filesMapObj = new HashMap<String, ArrayList<String>>();
	
	public static HashMap<String, ArrayList<String>> getFiles() {
		return filesMapObj;
	}

	public static void main(String[] args) throws IOException {
		ServerSocket serverSocket = new ServerSocket(SERVER_PORT);
		System.out.println("Server is running on "+ SERVER_PORT);
		while(true) {
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
                Thread threadReq = new ClientRequestHandler(socket, objInputStream, objOutputStream); 
                // Invoking the start() method 
                threadReq.start();
            }  catch (IOException e) {
                e.printStackTrace();
            }  finally {
            	socket.close();
            }
        }
	}
	
}

class ClientRequestHandler extends Thread { 
    private final Socket threadSocket;
    private final ObjectInputStream objInputStream; 
    private final ObjectOutputStream objOutputStream;
  
    public ClientRequestHandler(Socket thrSocket, ObjectInputStream objInputStream, ObjectOutputStream objOutputStream) { 
        this.threadSocket = thrSocket; 
        this.objInputStream = objInputStream;
        this.objOutputStream = objOutputStream;
    } 
  
    @Override
    public void run() {
    	try {
    		Object obj = this.objInputStream.readObject();
    		if(obj instanceof PeerRegister) {
    			PeerRegister peerInfo = (PeerRegister) obj;
    			HashMap<String, ArrayList<String>> mapObj = IndexServer.getFiles();
//    			String ipKey = this.threadSocket.getInetAddress().toString();
    			String hostName = this.threadSocket.getInetAddress().getHostName();
    			
        		mapObj.put(hostName, peerInfo.getFileNames());
        		
        		System.out.println("Registered Peer ->>  " + hostName + ":" + peerInfo.getFileNames().toString());
    		} else if(obj instanceof PeerFileLookup) {
    			PeerFileLookup lookupObj = (PeerFileLookup) obj;
    			String fileName = lookupObj.getFileName();
    			HashMap<String, ArrayList<String>> mapObj = IndexServer.getFiles();
    			
    			ArrayList<String> retList = new ArrayList<String>();
    			for (HashMap.Entry<String, ArrayList<String>> entry : mapObj.entrySet()) {
    				if(entry.getValue().contains(fileName) == true) {
    					retList.add(entry.getKey());
    				}
    			}
    			this.objOutputStream.writeObject(retList);
    			this.objOutputStream.flush();
    		} else {
    			System.out.println("Invalid request arguments received");
    		}
    		
    		this.objInputStream.close();
    		this.objOutputStream.close();
    		this.threadSocket.close();
    	} catch (IOException | ClassNotFoundException e) {
    		e.printStackTrace();
    	}
    }
}

class PeerRegister implements Serializable {
	private static final long serialVersionUID = 7095249478609523764L;
	private ArrayList<String> fileNames;
	
	public ArrayList<String> getFileNames() {
		return fileNames;
	}
    
    public PeerRegister(ArrayList<String>fileNames) {
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
 */