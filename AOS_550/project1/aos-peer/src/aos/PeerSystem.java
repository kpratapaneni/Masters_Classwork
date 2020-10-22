package aos;

import java.util.ArrayList;
import java.util.Scanner;
import java.util.stream.Stream;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.OutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.Serializable;

public class PeerSystem {
//	private static final String SERVER_HOST_NAME = "my_index_server";
	private static final String SERVER_HOST_NAME = "129.114.32.159";
	private static final int SERVER_PORT = 5056;
	private static final int PEER_PORT = 6066;

	private static final String FILES_PATH = "/srv/shared/";

	public static void main(String[] args) {
		boolean isBatchMode = (args.length == 4);
		if (isBatchMode == false) {
			registry();
		}
		Thread clientSocketThread = new Thread() {
			public void run() {
				ServerSocket clientPeerSocket = null;
				try {
					clientPeerSocket = new ServerSocket(PEER_PORT);
					while (true) {
						Socket socket = clientPeerSocket.accept();

						System.out.println("A new peer is connected : " + socket);

						ObjectInputStream fileInput = new ObjectInputStream(socket.getInputStream());
						String fileName = fileInput.readUTF();
						System.out.println("Sending file info : " + fileName);
						/*********** File Sharing ***********/
						File fileToShare = new File(FILES_PATH, fileName);
						long fileLength = fileToShare.length();
						byte[] fileBytesLen = new byte[(int) fileLength];

						FileInputStream fileInputStream = new FileInputStream(fileToShare);
						BufferedInputStream bufInputStream = new BufferedInputStream(fileInputStream);
						bufInputStream.read(fileBytesLen, 0, fileBytesLen.length);

						OutputStream outputStream = socket.getOutputStream();
						System.out.println("Sending " + fileName + "(" + fileBytesLen.length + " bytes)");
						outputStream.write(fileBytesLen, 0, fileBytesLen.length);
						outputStream.flush();
						System.out.println("Done.");
						/*********** File Sharing ***********/

						outputStream.close();
						fileInputStream.close();
						bufInputStream.close();
						socket.close();
					}
				} catch (IOException e) {
					e.printStackTrace();
				} finally {
					if (clientPeerSocket != null) {
						try {
							clientPeerSocket.close();
						} catch (IOException e) {
							e.printStackTrace();
						}
					}
				}
			}
		};

		Thread clientOptions = new Thread() {
			public void run() {
				while (true) {
					System.out.println("Select the choices as shown below");
					System.out.println("1. lookup a file");
					System.out.println("2. exit");

					System.out.print("Enter your choice: ");
					int choice = new Scanner(System.in).nextInt();

					if (choice == 1) {
						System.out.print("Enter file name: ");
						String fileName = new Scanner(System.in).nextLine();
						ArrayList<String> peerHosts = search(fileName);
						obtain(peerHosts, fileName, true);
					} else if (choice == 2) {
						System.exit(0);
					} else {
						System.out.println("Invalid option");
					}
				}
			}
		};

		Thread batchClientOptions = new Thread() {
			private static final String SEARCH_ENUM = "search";
			private static final double SECS_CONVERTER = 1_000_000_000.0;

			public void run() {
				String scalingInputFileName = args[0];
				String batchType = args[1];
				String scalingOutputFileName = args[2];
				String testName = args[3];

				ArrayList<String> fileNames = new ArrayList<String>();

				try (Stream<String> stream = Files.lines(Paths.get(scalingInputFileName))) {
					stream.forEach(line -> fileNames.add(line));
				} catch (IOException e) {
					e.printStackTrace();
				}

				double secondsDiff = 0;
				if (SEARCH_ENUM.equals(batchType)) {
					long start = System.nanoTime();
					for (String fileName : fileNames) {
						search(fileName);
					}
					long end = System.nanoTime();
					secondsDiff = (end - start) / SECS_CONVERTER;
				} else {
					long start = System.nanoTime();
					for (String fileName : fileNames) {
						ArrayList<String> peerHosts = search(fileName);
						obtain(peerHosts, fileName, false);
					}
					long end = System.nanoTime();
					secondsDiff = (end - start) / SECS_CONVERTER;
				}
				secondsDiff = fileNames.size() / secondsDiff;

				BufferedWriter writer = null;
				try {
					writer = new BufferedWriter(new FileWriter(scalingOutputFileName, true));
					String textToAppend = testName + " " + String.valueOf(secondsDiff) + "\n";
					writer.write(textToAppend);
				} catch (IOException e) {
					e.printStackTrace();
				} finally {
					if (writer != null) {
						try {
							writer.close();
						} catch (IOException e) {
							e.printStackTrace();
						}
					}
				}

				System.out.println("Time took to " + batchType + "  " + fileNames.size() + ">>" + ">>" + secondsDiff);
			}
		};

		clientSocketThread.start();
		if (isBatchMode == true) {
			batchClientOptions.start();
		} else {
			clientOptions.start();
		}

	}

	private static void registry() {
		Socket connectedSocket = null;
		ObjectOutputStream objOutputStream = null;
		ObjectInputStream objInputStream = null;
		try {
			File[] files = new File(FILES_PATH).listFiles();
			ArrayList<String> fileNames = new ArrayList<String>();
			for (File file : files) {
				if (file.isFile()) {
					fileNames.add(file.getName());
				}
			}
			System.out.println("file names:" + String.valueOf(fileNames));
			PeerRegister peerInfo = new PeerRegister(fileNames);

			System.out.println("Connecting to " + SERVER_HOST_NAME + " on port " + SERVER_PORT);
			connectedSocket = new Socket(SERVER_HOST_NAME, SERVER_PORT);
			System.out.println("Connected to " + connectedSocket.getRemoteSocketAddress());

			objOutputStream = new ObjectOutputStream(connectedSocket.getOutputStream());
			objOutputStream.writeObject(peerInfo);
			objOutputStream.flush();
			
			objInputStream = new ObjectInputStream(connectedSocket.getInputStream());
			Object message = objInputStream.readObject();

			System.out.println("Successfully registered " + fileNames.size() + " files with the Index Server");
			System.out.println("Server message" + String.valueOf(message));
		} catch (IOException e) {
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		} finally {
			try {
				if (objOutputStream != null) {
					objOutputStream.close();
				}
				if (connectedSocket != null) {
					connectedSocket.close();
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	public static ArrayList<String> search(String fileName) {
		ObjectOutputStream objOutputStream = null;
		ObjectInputStream inputObjStream = null;
		Socket serverSocket = null;
		ArrayList<String> peerHosts = new ArrayList<String>();
		try {
			System.out.println("Connecting to " + SERVER_HOST_NAME + " on port " + SERVER_PORT);
			serverSocket = new Socket(SERVER_HOST_NAME, SERVER_PORT);
			System.out.println("Connected to " + serverSocket.getRemoteSocketAddress());

			// input data
			objOutputStream = new ObjectOutputStream(serverSocket.getOutputStream());
			objOutputStream.writeObject(new PeerFileLookup(fileName));
			objOutputStream.flush();
			// input data

			// output data
			inputObjStream = new ObjectInputStream(serverSocket.getInputStream());
			// output data

			peerHosts = (ArrayList<String>) inputObjStream.readObject();

			System.out.println("Below are the peers list which has file");
			System.out.println(String.valueOf(peerHosts));

		} catch (IOException e) {
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		} finally {
			try {
				if (objOutputStream != null) {
					objOutputStream.close();
				}
				if (inputObjStream != null) {
					inputObjStream.close();
				}
				if (serverSocket != null) {
					serverSocket.close();
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		return peerHosts;
	}

	public static void obtain(ArrayList<String> peerHosts, String fileName, boolean userCmdLineMode) {
		if (peerHosts == null || peerHosts.size() == 0) {
			System.out.println("File does not exist in any peer.");
			return;
		}

		int peerHostChoice;
		if (peerHosts.size() == 1) {
			System.out.println("The file exists only in one peer so directly downloading");
			peerHostChoice = 0;
		} else if (userCmdLineMode == true && peerHosts.size() > 1) {
			System.out.println("Select the choices as shown below");
			for (int i = 0; i < peerHosts.size(); i++) {
				System.out.println(i + 1 + " for " + peerHosts.get(i));
			}
			peerHostChoice = new Scanner(System.in).nextInt();
			peerHostChoice =  peerHostChoice-1;
		} else {
			peerHostChoice = 0;
		}

		String peerHostName = "";
		try {
			peerHostName = peerHosts.get(peerHostChoice);
		} catch (IndexOutOfBoundsException e) {
			e.printStackTrace();
			System.out.println("Since user has given wrong index, system is considering the first peer");
			peerHostName = peerHosts.get(0);
		}

		System.out.println("Fetching file from " + peerHostName + " downloading it to " + FILES_PATH);

		// Here the real game starts
		Socket peerSocket = null;
		ObjectOutputStream fileOutput = null;
		FileOutputStream fileOutputStream = null;
		InputStream inputStream = null;
		BufferedOutputStream bufferedOutputStream = null;
		try {
			peerSocket = new Socket(peerHostName, PEER_PORT);
			fileOutput = new ObjectOutputStream(peerSocket.getOutputStream());
			fileOutput.writeUTF(fileName);
			fileOutput.flush();

			inputStream = peerSocket.getInputStream();
			fileOutputStream = new FileOutputStream(FILES_PATH + fileName);
			bufferedOutputStream = new BufferedOutputStream(fileOutputStream);

			byte[] array = inputStream.readAllBytes();
			bufferedOutputStream.write(array, 0, array.length);
			bufferedOutputStream.flush();
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			try {
				if (fileOutput != null) {
					fileOutput.close();
				}
				if (fileOutputStream != null) {
					fileOutputStream.close();
				}
				if (bufferedOutputStream != null) {
					bufferedOutputStream.close();
				}
				if (inputStream != null) {
					inputStream.close();
				}
				if (fileOutput != null) {
					fileOutput.close();
				}
				if (peerSocket != null) {
					peerSocket.close();
				}
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
