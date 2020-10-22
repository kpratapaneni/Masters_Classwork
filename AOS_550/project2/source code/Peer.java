import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.ConcurrentHashMap;
import java.util.stream.*;
import java.nio.file.Files;
import java.nio.file.Paths;

/*
 * @author Vaishnavi Manjunath
 * @author Kalpana Pratapaneni
 * @author Kavya Ravella
 */

public class Peer 
{
	public static void readFile(String config)
	{
		FileWriter writer = null;
		String s = new String();
		try
		{
			//Read the configuration file and fetch the neighbouring peers. If configuration file is not found
			//error message will be printed
			writer = new FileWriter("./Log.txt",true);
			
			File inputConfigFile = new File("./config.txt");
			if(inputConfigFile.isFile() && inputConfigFile.exists())
			{
				InputStreamReader read = new InputStreamReader(new FileInputStream(inputConfigFile));
				BufferedReader br = new BufferedReader(read);
				while((s = br.readLine()) != null)
				{
					Peer_Data.source.peerNum++;
					String inputData[] = s.split(" ");
					if(Peer_Data.source.nodeList.peerName.equals(inputData[0]))
					{
						Peer_Data.source.nodeList.IP = inputData[1];
						Peer_Data.source.nodeList.port = Integer.parseInt(inputData[2]);
						if(inputData.length>2) 
						for(int i = 3; i < inputData.length; i++)
						{
							BufferedReader reader = new BufferedReader(new FileReader(inputConfigFile));
							while((s = reader.readLine()) != null)
							{
								String comp[] = s.split(" ");
								if(inputData[i].equals(comp[0]))
								{
									Node node = new Node(comp[0], comp[1], Integer.parseInt(comp[2]));
									Peer_Data.source.neighbor.add(node);
									writer.write(Peer_Data.source.nodeList.peerName + " adjacent peer data:");
									writer.write(node.peerName + " ");
								}
							}
						}
						writer.write("\t\n");
					}
				}
			}
			else
			{
				System.out.println("Configuration file is not present. Kindly place it and re-run!");
				writer.write("Configuration file is not present. Kindly place it and re-run!\r\n");
			}		
			writer.close();
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
	}
	
	public static void register_file(String fileName)
	{
		FileWriter writer = null;

		try
		{		
			File file = new File(Peer_Data.source.directory + File.separator + fileName);
			if(file.exists())
			{
				writer = new FileWriter("./Log.txt",true);	
				Peer_Data.source.fileList.add(fileName);
				System.out.println("File "+fileName + " is registered in the local peer !");
				DateFormat pattern = new SimpleDateFormat("MM-dd-yyyy HH:mm:ss");
				String time = pattern.format(new Date());
				writer.write("File "+fileName + " is registered in local peer at time: " +time + "\r\n");
				writer.close();	
				
			}
			else
			{
				System.out.println(fileName+" doesnot exists in the peer!");
			}	

		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
	}
	
	public static void deregister_file(String fileName)
	{
		FileWriter writer = null;
		try
		{	
			writer = new FileWriter("./Log.txt",true);
			Peer_Data.source.fileList.remove(fileName);		
			DateFormat pattern = new SimpleDateFormat("MM-dd-yyyy HH:mm:ss");
			String time = pattern.format(new Date());
			writer.write("File "+fileName + " is deregistered  from the index server at time: " +time + "\r\n");
			writer.close();	
			System.out.println("File "+fileName + " is deregistered from peer!");
			
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
	}
	
	public void Search_inpfile(String directory)
	{
		File file = new File(directory);
		if(!file.exists() && !file.isDirectory())
		{        
		    file.mkdir();    
		} 
		String inpfile[];
		inpfile = file.list();
		if(inpfile.length!=0)
		{
			for(int i = 0; i<inpfile.length; i++)
			{
				register_file(inpfile[i]);
			}
		}
	}


	public void connect(channel action, boolean isBatchMode, String scalingInputFileName, String batchType, String scalingOutputFileName, String testName) throws IOException {
		
		boolean exit = false;	
		String fileName = null;
		BufferedReader UserInputReader = new BufferedReader(new InputStreamReader(System.in));
		while(!exit)
		{
			System.out.println("\n1.Set up peer \n2.Search and download file in required peer \n3.Exit");
			switch(Integer.parseInt(UserInputReader.readLine()))
			{
			case 1:
			{					
				System.out.println("Enter the peer name in the format of p1, p2, etc :");
				Peer_Data.source.nodeList.peerName = UserInputReader.readLine();
				Peer_Data.initial();
				readFile(Peer_Data.source.config);
				ServerSocket server = null;
			    try
			    {
			    	server = new ServerSocket(Peer_Data.source.nodeList.port);
			    	System.out.println("\nServer is up and running...!");
			    	new PThread(server, action);
			    	System.out.println("\nAll files of this server is registered!");
			    	Search_inpfile(Peer_Data.source.directory);	
			    	
			    	/*
					 * Run batch Mode Thread
					 */
			    	
			    	if (isBatchMode == true) {
						// run the thread
			    		System.out.println("control is in this if method");
						BatchClientOptions batchThread = new BatchClientOptions(action, scalingInputFileName, batchType, scalingOutputFileName, testName);
						batchThread.start();
			    	}
			    }
			    catch(IOException e)
			    {
			    	e.printStackTrace();
			    }
				break;
			}
			
			case 2:
			{
				Peer_Data.destination.destPeer = new ArrayList<Node>();
				Peer_Data.source.hitQueryRequest = 0;
				System.out.println("Enter the file name to be searched and downloaded:");
				fileName = UserInputReader.readLine();				
				int num = Peer_Data.source.messageNum + 1;
				MessageID seq_num = new MessageID(num, Peer_Data.source.nodeList);
				action.query(seq_num, Peer_Data.source.TTL-1, fileName);
				
				long runtime = 0;
				long start = System.currentTimeMillis();
				
				while(runtime<Peer_Data.source.cutoffTime)
				{
					long end = System.currentTimeMillis();
					runtime = end - start;
				}
				
				if(Peer_Data.destination.destPeer.size()!=0)
				{
					int index = 0;
					int indexNum = 0;
					System.out.println(fileName + " found on below peers!");
					System.out.println("\n1.Download the file \n2.Cancel and back");
					
					int choice = Integer.parseInt(UserInputReader.readLine());
					
					System.out.println(choice);
					
					if (choice == 1)
					{
						System.out.println("The peer list is:");
						for(int i=0; i<Peer_Data.destination.destPeer.size(); i++)
						{
							index = i + 1;
							System.out.println(index + ":" + Peer_Data.destination.destPeer.get(i).IP + " " + Peer_Data.destination.destPeer.get(i).port);
						}
						System.out.println("Choose which peer to download the file:");
						indexNum = Integer.parseInt(UserInputReader.readLine());
						new DThread(Peer_Data.source.nodeList.port+1,fileName);
						action.downLoad(fileName, indexNum, Peer_Data.source.nodeList.IP, Peer_Data.source.nodeList.port+1);					
						//break;
					}
					//break;			
				}
				else
				{
					System.out.println(fileName + " is not found on any connected peers. Please check and provide the file which is present!");
				}
				break;
			}
			
			case 3:
			{
				exit = true;
				System.exit(0);
				break;
			}

			default:
				break;
			}
		}	
	}
	
		
	public static void main(String args[])
	{
		Peer peer = new Peer();
		channel action = new channel();
		new WeThread(Peer_Data.source.directory);
		try 
		{
			boolean isBatchMode = (args.length == 4);
			String scalingInputFileName = null;
			String batchType = null;
			String scalingOutputFileName = null;
			String testName = null;
			if (isBatchMode == true) {
				scalingInputFileName = args[0];
				batchType = args[1];
				scalingOutputFileName = args[2];
				testName = args[3];
			}
			peer.connect(action, isBatchMode, scalingInputFileName, batchType, scalingOutputFileName, testName);	
		} 
		catch (IOException e) 
		{
			e.printStackTrace();
		}	
	}
}

class WeThread extends Thread {
	String directory = null;

	public WeThread(String directory){
		this.directory = directory;
		
		start();
	}
	
	public void run()
	{
		Timer timer = new Timer();
		timer.schedule(new TimerTask()
		{
			@Override
			public void run() 
			{	
				if(Peer_Data.source.fileList.size()!=0)
				{
					for(int i = 0; i < Peer_Data.source.fileList.size(); i++)
					{
						File file = new File(directory + File.separator + Peer_Data.source.fileList.get(i));
						if(!file.exists())
						{
							System.out.println(Peer_Data.source.fileList.get(i)+" is removed!");
							Peer.deregister_file(Peer_Data.source.fileList.get(i));	
						}
					}
				}
			}
		}, 1000, 100);    
	}
}

class Peer_Data 
{
	public static class source
	{
		public static int messageNum;
		public static int peerNum;
		public static int hitQueryRequest;
		public static int TTL = 3;	
		public static int cutoffTime = 3000;
		public static Node nodeList = new Node();
		public static String directory = "./Files";
		public static String config = "./config.txt";
		public static ArrayList<Node> neighbor = new ArrayList<Node>();
		public static ArrayList<String> fileList = new ArrayList<String>();
		public static ConcurrentHashMap<Integer,MessageID> messageTable = new ConcurrentHashMap<Integer,MessageID>();
	}
	
	public static class destination
	{
		public static ArrayList<Node> destPeer = new ArrayList<Node>();
	}
	
	public static void initial()
	{
		Peer_Data.source.messageNum = 0;
		Peer_Data.source.peerNum = 0;
		Peer_Data.source.hitQueryRequest = 0;
		Peer_Data.source.neighbor = new ArrayList<Node>();
		Peer_Data.source.messageTable = new ConcurrentHashMap<Integer,MessageID>();
		Peer_Data.destination.destPeer = new ArrayList<Node>();
	}
}

class Node implements Serializable
{
	private static final long serialVersionUID = 1L;	
	public String peerName;
	public String IP;
	public int port;
	
	public Node()
	{
		
	}
	
	public Node(String IP, int port)
	{
		this.IP = IP;
		this.port = port;
	}
	
	public Node(String peerName, String IP, int port)
	{
		this.peerName = peerName;
		this.IP = IP;
		this.port = port;
	}

	public void NodeInfo() 
	{
		System.out.println(peerName + " " + IP + " " + port);
	}
}


class DThread extends Thread
{
	int port;
	String fileName;
	public DThread(int port, String fileName)
	{
		this.port = port;
		this.fileName = fileName;
		start();
	}
	
	public void run()
	{
		try 
		{
			ServerSocket server = new ServerSocket(port);
			Socket sc = server.accept();
			receiveFile(sc, fileName);
			sc.close();
			server.close();
			} 
		catch (IOException e) 
		{
			e.printStackTrace();
		} 
	}
	
	public static void receiveFile(Socket soc, String fileName) throws IOException
	{
		byte[] inputByte = null;  
        int length = 0;  
        DataInputStream dis = null;  
        FileOutputStream fos = null;  
        String filePath = "./Files/" + fileName;  
        try 
        {  
            dis = new DataInputStream(soc.getInputStream());  
            File f = new File("./Files");  
            if(!f.exists())
            {  
                f.mkdir();    
            }  
            fos = new FileOutputStream(new File(filePath));      
            inputByte = new byte[1024];     
            System.out.println("display file " + fileName);
            while ((length = dis.read(inputByte, 0, inputByte.length)) > 0) 
            {  
                fos.write(inputByte, 0, length);  
                fos.flush();      
            }  
            System.out.println("Finish receive:"+filePath);  
        } 
        finally 
        {  
            if (fos != null)  
                fos.close();  
            if (dis != null)  
                dis.close();  
            if (soc != null)  
                soc.close();   
        }  
	}
}

class BatchClientOptions extends Thread {
	private static final String SEARCH_ENUM = "search";
	private static final double SECS_CONVERTER = 1_000_000_000.0;

	String scalingInputFileName;
	String batchType;
	String scalingOutputFileName;
	String testName;
	channel action;

	public BatchClientOptions(channel action, String scalingInputFileName, String batchType,
			String scalingOutputFileName, String testName) {
		this.action = action;
		this.scalingInputFileName = scalingInputFileName;
		this.batchType = batchType;
		this.scalingOutputFileName = scalingOutputFileName;
		this.testName = testName;
	}

	public void run() {
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
				Peer_Data.destination.destPeer = new ArrayList<Node>();
				Peer_Data.source.hitQueryRequest = 0;
				int num = Peer_Data.source.messageNum + 1;
				MessageID seq_num = new MessageID(num, Peer_Data.source.nodeList);
				action.query(seq_num, Peer_Data.source.TTL - 1, fileName);
			}
			long end = System.nanoTime();
			secondsDiff = (end - start) / SECS_CONVERTER;
		} else {
			long start = System.nanoTime();
			for (String fileName : fileNames) {
				//Peer_Data.destination.destPeer = new ArrayList<Node>();
				Peer_Data.source.hitQueryRequest = 0;
				int num = Peer_Data.source.messageNum + 1;
				MessageID seq_num = new MessageID(num, Peer_Data.source.nodeList);
				action.query(seq_num, Peer_Data.source.TTL - 1, fileName);
				//if (Peer_Data.destination.destPeer.size() != 0) {
				int indexNum = 1;
				new DThread(Peer_Data.source.nodeList.port + 1, fileName);
				action.downLoad(fileName, indexNum, Peer_Data.source.nodeList.IP, ++Peer_Data.source.nodeList.port);					
				//}
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
}