Step1:
-----
	We have created 10 Virtual machines with names peer-1, peer-2, peer-3, peer-4.. and peer-0.

	create folders as shown below in each virtual machine
	$ cd /srv && sudo mkdir deployments && sudo mkdir shared && sudo chmod 777 shared/ && sudo chmod 777 deployments/

	And add security groups to communicate with each other on the specified ports.
	When running a peer, the first step must be selecting its name and based on the name(like p1), it will bind the port.

	To open port from all peers to other peer, we have configured
	Each peer with a TCP ingress rule port 8010 for p1, 8020 for p2 and so on.

Step 2:
------
	Copied all sources using SCP command to all machines.
	
	scp -i .ssh/group1.pem -r ./peer-1 group1@<peer-ip-address>:/srv/deployments

	scp -i .ssh/group1.pem ./create_100k_10kb_text_files.sh kpst@129.114.32.209:/srv/deployments/
	scp -i .ssh/group1.pem ./create_10_100mb_binary_files.sh kpst@129.114.32.209:/srv/shared
	scp -i .ssh/group1.pem ./create_1k_1mb_text_files.sh kpst@129.114.32.209:/srv/shared

	Repeated the command for all the peers.

Step 3:
------
	Created random text and binary files using shell scripts. To make files unique to each peer, script takes the peer name from command line.
	$ cd /srv/deployments/
	$ rm -rf peer*.txt *.bin
		- Removes the files if there are any test files exists.

	-> Creating 100K: 10KB text files
	$ cd /srv/deployments/peer-1/Files
	$ sh create_100k_10kb_text_files.sh
		Enter Peer Unique Id: peer1

		Then it creates files as shown below.
	$ ls peer1-file-*.txt

		peer1-file-1-10kb.txt
		peer1-file-2-10kb.txt
		.....................
		peer1-file-100000-10kb.txt
	
	-> Creating 10: 100MB binary file
	$ sh create_10_100mb_binary_files.sh
	  Enter Peer Unique Id: peer1

	  Then it creates files as shown below.
	  $ ls *.bin

		peer1-file-1-100mb.bin
		peer1-file-2-100mb.bin
		....................
		peer1-file-10-100mb.bin

Step 4:
------
	Compiling Source Code:
	Go to /srv/deployments/peer-1
	$ javac *.java
	
	Note: We have used “make” file to compile
	
Step 5:
------
	Run the Peer using the below command:
	$ java Peer

	Sample Output:
	1.Set up peer 
	2.Search and download file in required peer 
	3.Exit
	1
	Enter the peer name in the format of p1, p2, etc :
	p1

	Server is up and running...!
	All files of this server is registered!
	File peer1-file-4505-10kb.txt is registered in the local peer !
	File peer1-file-919-10kb.txt is registered in the local peer !
	File peer1-file-3854-10kb.txt is registered in the local peer !
	File peer1-file-589-10kb.txt is registered in the local peer !
	File peer1-file-4995-10kb.txt is registered in the local peer !
	File peer1-file-6664-10kb.txt is registered in the local peer !
	File peer1-file-6402-10kb.txt is registered in the local peer !
	File peer1-file-6892-10kb.txt is registered in the local peer !
	File peer1-file-1953-10kb.txt is registered in the local peer !
	File peer1-file-4763-10kb.txt is registered in the local peer !
	File peer1-file-9825-10kb.txt is registered in the local peer !
	File peer1-file-6403-10kb.txt is registered in the local peer !
	File peer1-file-4762-10kb.txt is registered in the local peer !
	File peer1-file-6893-10kb.txt is registered in the local peer !
	File peer1-file-1952-10kb.txt is registered in the local peer !
	File peer1-file-918-10kb.txt is registered in the local peer !
	File peer1-file-4504-10kb.txt is registered in the local peer !
	File peer1-file-6665-10kb.txt is registered in the local peer !
	File peer1-file-3855-10kb.txt is registered in the local peer !
	File peer1-file-4994-10kb.txt is registered in the local peer !
	File peer1-file-588-10kb.txt is registered in the local peer !
	File peer1-file-9824-10kb.txt is registered in the local peer !
	File peer1-file-321-10kb.txt is registered in the local peer !
	File peer1-file-147-10kb.txt is registered in the local peer !
	File peer1-file-5303-10kb.txt is registered in the local peer !
	File peer1-file-7062-10kb.txt is registered in the local peer !
	File peer1-file-7204-10kb.txt is registered in the local peer !
	File peer1-file-5165-10kb.txt is registered in the local peer !
	File peer1-file-146-10kb.txt is registered in the local peer !
	File peer1-file-320-10kb.txt is registered in the local peer !
	File peer1-file-7205-10kb.txt is registered in the local peer !
	File peer1-file-5164-10kb.txt is registered in the local peer !
	File peer1-file-5302-10kb.txt is registered in the local peer !
	File peer1-file-7063-10kb.txt is registered in the local peer !
	File peer1-file-883-10kb.txt is registered in the local peer !
	File peer1-file-8911-10kb.txt is registered in the local peer !
	File peer1-file-8581-10kb.txt is registered in the local peer !
	File peer1-file-413-10kb.txt is registered in the local peer !
	File peer1-file-1459-10kb.txt is registered in the local peer !


	1.Set up peer 
	2.Search and download file in required peer 
	3.Exit
	2
	Enter the file name to be searched and downloaded:
	peer-common-file-6214-10kb.txt
	peer-commone-6214-10kb.txt is found on any below peers.
	1.Download the file
	2.Cancel and back
	1
	The peer list is:
	1 127.0.0.1 8020
	2 127.0.0.1 8030
	Choose which peer to download the file:
	peer-commone-6214-10kb.txt Successfully downloaded!!



Experiments:
-----------
To conduct experiments for query and download, we run the code in batch mode. Before running the experiment, initiate each peer with appropriate name using command line mode.

Four command line arguments are required to start the process.

1) File names input file path
2) type of operation
	- search = for query
	- download = for download
3) output file path
4) y scale label to be printed on output txt

$ java Peer /srv/deployments/peer-1/experiment/peer-1-input.txt search /srv/deployments/peer-1/experiment/output.txt 1-node

Sample Output:
-------------
2019-11-11 22:09:53		Send:hitQuery 605 peer9-file-1-10kb.txt p5	
2019-11-11 22:09:53		Send:query 3044 peer2-file-10-10kb.txt p5	
2019-11-11 22:09:53		Receive: query 609 peer2-file-10-10kb.txt p1	
2019-11-11 22:09:53		Send:hitQuery 609 peer2-file-10-10kb.txt p5	
2019-11-11 22:09:53		Receive: query 637 peer5-file-10-10kb.txt p1	
2019-11-11 22:09:53		Send:hitQuery 637 peer5-file-10-10kb.txt p5	
2019-11-11 22:09:53		Receive: query 619 peer7-file-1-10kb.txt p1	
2019-11-11 22:09:53		Send:hitQuery 619 peer7-file-1-10kb.txt p5	
2019-11-11 22:09:53		Receive: query 631 peer3-file-10-10kb.txt p1	
2019-11-11 22:09:53		Send:hitQuery 631 peer3-file-10-10kb.txt p5	
2019-11-11 22:09:53		Send:query 3051 peer8-file-1-10kb.txt p5	
2019-11-11 22:09:53		Receive: query 581 peer8-file-1-10kb.txt p1	
2019-11-11 22:09:53		Send:hitQuery 581 peer8-file-1-10kb.txt p5	
2019-11-11 22:09:53		Send:query 3053 peer0-file-1-10kb.txt p5	
2019-11-11 22:09:53		Receive: query 650 peer6-file-10-10kb.txt p1	
2019-11-11 22:09:53		Send:hitQuery 650 peer6-file-10-10kb.txt p5	
2019-11-11 22:09:53		Receive: query 685 peer4-file-10-10kb.txt p1	
2019-11-11 22:09:53		Send:hitQuery 685 peer4-file-10-10kb.txt p5	
2019-11-11 22:09:53		Receive: query 720 peer8-file-10-10kb.txt p1	
2019-11-11 22:09:53		Send:hitQuery 720 peer8-file-10-10kb.txt p5	
2019-11-11 22:09:53		Receive: query 722 peer7-file-10-10kb.txt p1	
2019-11-11 22:09:53		Send:hitQuery 722 peer7-file-10-10kb.txt p5	
