Step1:
-----
	We have created 5 Virtual machines with names peer-1, peer-2, peer-3, peer-4 and IndexServer.
	peer-1(Instance Name: Group-9-Peer-1):
	Private IP: 192.168.4.19
	Floating IP: 129.114.32.209

	peer-2(Instance Name: Group-9-Peer-2):
	Private IP: 192.168.4.21
	Floating IP: 129.114.32.187

	peer-3(Instance Name: Group-9-Peer-3):
	Private IP: 192.168.4.20
	Floating IP: 129.114.32.163

	peer-4(Instance Name: Group-9-Peer-4):
	Private IP: 192.168.4.17
	Floating IP: 129.114.32.161

	IndexServer(Instance Name: Group-9-IndexServer):
	Private IP: 192.168.4.18
	Floating IP: 129.114.32.159

	And create folders as shown below
	$ cd /srv && sudo mkdir deployments && sudo mkdir shared && sudo chmod 777 shared/ && sudo chmod 777 deployments/

	And add security groups to communicate with each other on the specified ports.
	Server runs on 5056 port and all peers runs on 6066 port.

	To open port from all peers to Index Server, we have configured
	group-9-index-server with a TCP ingress rule port 5066

	To open port from one peer to other peers, we have configured
	group-9-peer-group with a TCP ingress rule port 6066

Step 2:
------
	Copied all sources using SCP command to all machines.
	
	scp -i .ssh/kpst.pem -r ./index-server kpst@129.114.32.209:/srv/deployments
	scp -i .ssh/kpst.pem -r ./aos-peer kpst@129.114.32.209:/srv/deployments

	scp -i .ssh/kpst.pem ./create_100k_10kb_text_files.sh kpst@129.114.32.209:/srv/shared
	scp -i .ssh/kpst.pem ./create_10_100mb_binary_files.sh kpst@129.114.32.209:/srv/shared
	scp -i .ssh/kpst.pem ./create_1k_1mb_text_files.sh kpst@129.114.32.209:/srv/shared

	Repeated the command for all servers

Step 3:
------
	Created random text and binary files using shell scripts. To make files unique to each peer, script asks the name of the peer from command line.
	$ cd /srv/shared
	$ rm -rf peer*.txt *.bin
		- Removes the files if there are any test files exists.

	-> Creating 100K: 10KB text files
	$ sh create_100k_10kb_text_files.sh
		Enter Peer Unique Id: peer1

		Then it creates files as shown below.
	$ ls peer1-file-*.txt

		peer1-file-1-10kb.txt
		peer1-file-2-10kb.txt
		.....................
		peer1-file-100000-10kb.txt

	-> Creating 1K: 1MB text files
	$ sh create_1k_1mb_text_files.sh
	$ Enter Peer Unique Id: peer1

	Then it creates files as shown below.
	$ ls peer1-file-*.txt

	  peer1-file-1-1mb.txt
	  peer1-file-2-1mb.txt
	  ....................
	  peer1-file-1000-1mb.txt
	
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
	Compiling Server:
	Go to /srv/deployments
	$ cd /srv/deployments/
	$ ant jar

	Sample Output:
	Buildfile: /srv/deployments/index-server/build.xml

	clean:
	   [delete] Deleting directory /srv/deployments/index-server/dist
	    [mkdir] Created dir: /srv/deployments/index-server/dist

	compile:
	    [javac] Compiling 2 source files to /srv/deployments/index-server/dist

	jar:
	     [echo] Packaging Index Server into a single JAR at /srv/deployments/index-server/dist
	      [jar] Building jar: /srv/deployments/index-server/dist/IndexServer.jar

	BUILD SUCCESSFUL
	Total time: 1 second

	Note: 	ant clean - cleans the compiled sources,
			ant compile - cleans and compiles the sources into dist folder,
			ant jar - cleans, compiles and makes jar file into dist folder.
			These instructions are in /srv/deployments/index-server/build.xml file.

Step 5:
------
	Compiling peer and making jar file.

	$ cd /srv/deployments/aos-peer
	$ ant jar
		
	Sample Output:
	Buildfile: /srv/deployments/aos-peer/build.xml

	clean:
	   [delete] Deleting directory /srv/deployments/aos-peer/dist
		[mkdir] Created dir: /srv/deployments/aos-peer/dist

	compile:
		[javac] Compiling 2 source files to /srv/deployments/aos-peer/dist
		[javac] Note: /srv/deployments/aos-peer/src/aos/PeerSystem.java uses unchecked or unsafe operations.
		[javac] Note: Recompile with -Xlint:unchecked for details.
		[javac] Creating empty /srv/deployments/aos-peer/dist/aos/package-info.class

	jar:
		 [echo] Packaging Peer System into a single JAR at /srv/deployments/aos-peer/dist
		  [jar] Building jar: /srv/deployments/aos-peer/dist/PeerSystem.jar

	BUILD SUCCESSFUL
	Total time: 1 second

	Note: 	ant clean - cleans the compiled sources,
			ant compile - cleans and compiles the sources into dist folder,
			ant jar - cleans, compiles and makes jar file into dist folder.
			These instructions are in /srv/deployments/aos-peer/build.xml file.

	Note*:
	========
	The server IP is configured in the Peer System directory. Please change it in case it runs on a different virtual machine and compile it using "ant jar" command.

Step 6:
------
	Run the Server using the below command:
	$ java -jar dist/IndexServer.jar

	Sample Output:
	Server is running on 5056
	A new client is connected : Socket[addr=/129.114.32.209,port=53510,localport=5056]
	Assigning a thread for this client request
	kvm-dyn-129-114-32-209.tacc.chameleoncloud.org is registering all its files.
	Registered Peer ->>  kvm-dyn-129-114-32-209.tacc.chameleoncloud.org with :101010 files
	A new client is connected : Socket[addr=/129.114.32.187,port=33074,localport=5056]
	Assigning a thread for this client request
	kvm-dyn-129-114-32-187.tacc.chameleoncloud.org is registering all its files.
	Registered Peer ->>  kvm-dyn-129-114-32-187.tacc.chameleoncloud.org with :101010 files
	A new client is connected : Socket[addr=/129.114.32.209,port=53512,localport=5056]
	Assigning a thread for this client request
	Searching file in the Server Indexpeer2-file-37681-10kb.txt
	Sending peers list to client [kvm-dyn-129-114-32-187.tacc.chameleoncloud.org]
	A new client is connected : Socket[addr=/129.114.32.209,port=53516,localport=5056]
	Assigning a thread for this client request
	Searching file in the Server Indexpeer2-file-4-100mb.bin
	Sending peers list to client [kvm-dyn-129-114-32-187.tacc.chameleoncloud.org]
	A new client is connected : Socket[addr=/129.114.32.209,port=53520,localport=5056]
	Assigning a thread for this client request
	Searching file in the Server Indexpeer2-file-156-1mb.txt
	Sending peers list to client [kvm-dyn-129-114-32-187.tacc.chameleoncloud.org]


Step 7:
------
	Run the Peer using the below command:
	$ java -jar dist/PeerSystem.jar

	Sample Output for Peer1(Say 129.114.32.209):
	--------------------------------------------
		file names:[peer1-file-32900-10kb.txt, peer1-file-97813-10kb.txt, peer1-file-8199-10kb.txt, peer1-file-32755-10kb.txt, peer1-file-83972-10kb.txt, peer1-file-85533-10kb.txt, peer1-file-2304-10kb.txt, peer1-file-81257-10kb.txt,
		............
		peer1-file-55859-10kb.txt, peer1-file-39510-10kb.txt, peer1-file-23188-10kb.txt, peer1-file-44460-10kb.txt, peer1-file-33005-10kb.txt]
		Connecting to 129.114.32.159 on port 5056
		Connected to /129.114.32.159:5056
		Successfully registered 101010 files with the Index Server
		Server messageDone
		Select the choices as shown below
		1. lookup a file
		2. exit
		Enter your choice: 1
		Enter file name: peer2-file-37681-10kb.txt
		Connecting to 129.114.32.159 on port 5056
		Connected to /129.114.32.159:5056
		Below are the peers list which has file
		[kvm-dyn-129-114-32-187.tacc.chameleoncloud.org]
		The file exists only in one peer so directly downloading
		Fetching file from kvm-dyn-129-114-32-187.tacc.chameleoncloud.org downloading it to /srv/shared/
		Select the choices as shown below
		1. lookup a file
		2. exit
		Enter your choice: 1
		Enter file name: peer2-file-4-100mb.bin
		Connecting to 129.114.32.159 on port 5056
		Connected to /129.114.32.159:5056
		Below are the peers list which has file
		[kvm-dyn-129-114-32-187.tacc.chameleoncloud.org]
		The file exists only in one peer so directly downloading
		Fetching file from kvm-dyn-129-114-32-187.tacc.chameleoncloud.org downloading it to /srv/shared/
		Select the choices as shown below
		1. lookup a file
		2. exit
		Enter your choice: 1
		Enter file name: peer2-file-156-1mb.txt       
		Connecting to 129.114.32.159 on port 5056
		Connected to /129.114.32.159:5056
		Below are the peers list which has file
		[kvm-dyn-129-114-32-187.tacc.chameleoncloud.org]
		The file exists only in one peer so directly downloading
		Fetching file from kvm-dyn-129-114-32-187.tacc.chameleoncloud.org downloading it to /srv/shared/
		Select the choices as shown below
		1. lookup a file
		2. exit
		Enter your choice: 2


	Sample Output for Peer2(Say 129.114.32.187):
	--------------------------------------------
	file names:[peer2-file-16652-10kb.txt, peer2-file-359-1mb.txt, peer2-file-11510-10kb.txt, peer2-file-97286-10kb.txt, peer2-file-90533-10kb.txt, peer2-file-9585-10kb.txt, peer2-file-16658-10kb.txt, peer2-file-37601-10kb.txt, 
	..........
	peer2-file-68491-10kb.txt, peer2-file-23063-10kb.txt, peer2-file-77615-10kb.txt, peer2-file-24979-10kb.txt, peer2-file-44656-10kb.txt, peer2-file-64577-10kb.txt, peer2-file-31497-10kb.txt]
	Connecting to 129.114.32.159 on port 5056
	Connected to /129.114.32.159:5056
	Successfully registered 101010 files with the Index Server
	Server messageDone
	Select the choices as shown below
	1. lookup a file
	2. exit
	Enter your choice: A new peer is connected : Socket[addr=/129.114.32.209,port=56074,localport=6066]
	Sending file info : peer2-file-37681-10kb.txt
	Sending peer2-file-37681-10kb.txt(10240 bytes)
	Done.
	A new peer is connected : Socket[addr=/129.114.32.209,port=56078,localport=6066]
	Sending file info : peer2-file-4-100mb.bin
	Sending peer2-file-4-100mb.bin(33554431 bytes)
	Done.
	A new peer is connected : Socket[addr=/129.114.32.209,port=56082,localport=6066]
	Sending file info : peer2-file-156-1mb.txt
	Sending peer2-file-156-1mb.txt(1048576 bytes)
	Done.
	1
	Enter file name: peer1-file-90738-10kb.txtA new peer is connected : Socket[addr=/129.114.32.209,port=56088,localport=6066]
	Sending file info : peer2-file-68491-10kb.txt
	Sending peer2-file-68491-10kb.txt(10240 bytes)
	Done.

	Connecting to 129.114.32.159 on port 5056
	Connected to /129.114.32.159:5056
	Below are the peers list which has file
	[kvm-dyn-129-114-32-209.tacc.chameleoncloud.org]
	The file exists only in one peer so directly downloading
	Fetching file from kvm-dyn-129-114-32-209.tacc.chameleoncloud.org downloading it to /srv/shared/
	Select the choices as shown below
	1. lookup a file
	2. exit
	Enter your choice: 2



WEAK SCALING:
------------
Make the below directory to put all input and output files of weak scaling.
$ mkdir /srv/deployments/aos-peer/weak-scaling

Copy Input file using SCP command from local machine
$ scp -i .ssh/kpst.pem AOS550/group-9/pa1/weak-scaling/peer-1-input.txt kpst@129.114.32.209:/srv/deployments/aos-peer/weak-scaling
$ scp -i .ssh/kpst.pem AOS550/group-9/pa1/weak-scaling/peer-2-input.txt kpst@129.114.32.209:/srv/deployments/aos-peer/weak-scaling
$ scp -i .ssh/kpst.pem AOS550/group-9/pa1/weak-scaling/peer-4-input.txt kpst@129.114.32.209:/srv/deployments/aos-peer/weak-scaling

Run the Server using the above command
$ java -jar dist/IndexServer.jar

Run peers and register all files into Server Index. For weak scaling, the peer system will run in batch mode.
Four command line arguments are required to start the process.

1) File names input file path
2) type of operation
3) output file path
4) y scale label to be printed on output txt

Below is the command to run the PeerSystem in batch mode.
For One node,
$ java -jar dist/PeerSystem.jar /srv/deployments/aos-peer/weak-scaling/peer-1-input.txt search /srv/deployments/aos-peer/weak-scaling/weak-scaling-output.txt 1-node

For two nodes,
$ java -jar dist/PeerSystem.jar /srv/deployments/aos-peer/weak-scaling/peer-2-input.txt search /srv/deployments/aos-peer/weak-scaling/weak-scaling-output.txt 2-node


For four nodes,
$ java -jar dist/PeerSystem.jar /srv/deployments/aos-peer/weak-scaling/peer-4-input.txt search /srv/deployments/aos-peer/weak-scaling/weak-scaling-output.txt 4-node


Outputs will be redirected to weak-scaling-output.txt, which contains number of nodes and its output as req/sec.

Sample Output:
1-node 521.6327269837357
2-node 519.3023064480158
4-node 504.40082191460226


AVERAGE: 515.1119517821179
STANDARD DEVIATION: 7.6334327030249

plot.py is the file, which shows the output file in a graphical line representation.
# python3.7 weak-scaling/plot.py

STRONG SCALING:
------------
Make the below directory to put all input and output files of weak scaling.
$ mkdir /srv/deployments/aos-peer/strong-scaling
$ mkdir /srv/deployments/aos-peer/strong-scaling/100k-test && mkdir /srv/deployments/aos-peer/strong-scaling/1k-test && mkdir /srv/deployments/aos-peer/strong-scaling/10-test

- Make random input files.

Below is the command to run the PeerSystem in batch mode to test strong scaling.


100k-10kb files test:
====================
For two nodes,
$ java -jar dist/PeerSystem.jar /srv/deployments/aos-peer/strong-scaling/100k-test/peer2-m1.txt lookup /srv/deployments/aos-peer/strong-scaling/100k-test/output.txt 2-node
$ java -jar dist/PeerSystem.jar /srv/deployments/aos-peer/strong-scaling/100k-test/peer2-m1.txt lookup /srv/deployments/aos-peer/strong-scaling/100k-test/output.txt 2-node

For Four nodes
$ java -jar dist/PeerSystem.jar /srv/deployments/aos-peer/strong-scaling/100k-test/peer4-m1.txt lookup /srv/deployments/aos-peer/strong-scaling/100k-test/output.txt 4-node


Outputs will be redirected to output.txt, which contains number of nodes and its output as files/sec.

Sample Output:
2-node 49.75247062588155
4-node 58.30294798767889


AVERAGE: 54.02770930678
STANDARD DEVIATION: 4.2752386808987

plot.py is the file, which shows the output file in a graphical line representation.
# python3.7 strong-scaling/100k-test/plot.py


10k-1mb file test:
====================
For two nodes,
$ java -jar dist/PeerSystem.jar /srv/deployments/aos-peer/strong-scaling/1k-test/peer2-m1.txt lookup /srv/deployments/aos-peer/strong-scaling/1k-test/output.txt 2-node
$ java -jar dist/PeerSystem.jar /srv/deployments/aos-peer/strong-scaling/1k-test/peer2-m2.txt lookup /srv/deployments/aos-peer/strong-scaling/1k-test/output.txt 2-node

For Four nodes,
$ java -jar dist/PeerSystem.jar /srv/deployments/aos-peer/strong-scaling/1k-test/peer4-m1.txt lookup /srv/deployments/aos-peer/strong-scaling/1k-test/output.txt 4-node

- Repeat it for all nodes.


Outputs will be redirected to output.txt, which contains number of nodes and its output as files/sec.
Sample Output:
2-node 6.081803859615059
4-node 14.08218197483918


AVERAGE: 10.081992917227
STANDARD DEVIATION: 4.0001890576121

plot.py is the file, which shows the output file in a graphical line representation.
# python3.7 strong-scaling/1k-test/plot.py

10-100MB files test:
====================
For two nodes,
$ java -jar dist/PeerSystem.jar /srv/deployments/aos-peer/strong-scaling/10-test/peer2-m1.txt lookup /srv/deployments/aos-peer/strong-scaling/10-test/output.txt 2-node
$ java -jar dist/PeerSystem.jar /srv/deployments/aos-peer/strong-scaling/10-test/peer2-m2.txt lookup /srv/deployments/aos-peer/strong-scaling/10-test/output.txt 2-node

For Four nodes,
$ java -jar dist/PeerSystem.jar /srv/deployments/aos-peer/strong-scaling/10-test/peer4-m1.txt lookup /srv/deployments/aos-peer/strong-scaling/10-test/output.txt 4-node

- Repeat it for all nodes.


Outputs will be redirected to output.txt, which contains number of nodes and its output as files/sec.
Sample Output:
2-node 1.0219625819090363
4-node 1.1705918886241815


AVERAGE: 1.0962772352666
STANDARD DEVIATION: 0.074314653357573

plot.py is the file, which shows the output file in a graphical line representation.
# python3.7 strong-scaling/10-test/plot.py



Can you deduce that your P2P centralized system is scalable up to 4 nodes? Does it scale well for some file sizes, but not for others? Based on the data you have so far, what would happen if you had 1K peers with small, medium, and large files? What would happen if you had 1 billion peers?

ANS:
===
Yes, we can scale a P2P centralized system with 4 nodes. It will work for all file sizes, transfer speed may vary depending on the file size and network.

Based on the data and analysis, this system works efficiently for 1k peers with small, medium and large files. And also provides the high availability of data, but maintaining 1k peers is difficult.

In case of 1 billion peers, it is not efficient. Since the peer system has to maintain a high available hash table and finding a file key itself would take considerable processing time.