CS 525 - Spring 2020 - Assignment 1 - Storage Manager

------------------------- FILES INCLUDED IN THE ASSIGNMENT -------------------------
The codebase for assignment 1 includes the following files:

1. storage_mgr.c
2. storage_mgr.h
3. test_assign1_1.c
5. test_helper.h
6. dberror.c
7. dberror.h
8. Makefile
9. README.txt text file

------------------------- MAKEFILE AND SCRIPT EXECUTION -------------------------

Execute any one of the below commands to run the file:
- make
	- By default, it compiles the "test_assign1_1.c" and its dependency files.
- make tcase
	- It compiles the "test_assign1_1.c" and its dependency files.

Execution:
	To execute test cases for "test_assign1_1.c", run the below command.
	$./tcase

To clear the generated output files, use the command:"make clean"
	$make clean

------------------------- storage_mgr.c -------------------------
Below are the functions defined in storage_mgr.c. The interfaces are defined in the corresponding header file, storage_mgr.h
Note: In all the below functions, file handling variables like currPagePos and totalNumOfPages etc., are kept updated whenever the file contents are modified)


void initStorageManager (): 
	Creating an init function that can be used to declare any global variables

 (1). createPageFile (): 
	Creates a new file in write mode. Empty block is written with '\0'.

 (2). openPageFile ():
	If an input file exists, it opens the existing file.
	Else RC_FILE_NOT_FOUND error is thrown.
	If the file exists, it is opened in read/write mode. The file handler is updated with filename, total count of pages, and current page position.
	
 (3). closePageFile ():
	Invokes fclose() to close the file that is currently opened.

 (4). destroyPageFile ():
	Deletes the file.

 (5). readBlock():
	Uses "totalNumPages" to check if a page exists.  If it does, the "fseek" function is called, and seeks the file pointer to that page. 
	The file is read into the PageHandler from the seek-location until the total page size using "fread". If the page does not exist, the "NON_EXISTING_PAGE" error is returned.

 (6). getBlockPos():
	Returns curPagePos variable from the file handler. (current block position)

 (7). readFirstBlock ():
	Sets fseek to the position of the first byte of the first page, to read the first block of data

 (8). readPreviousBlock ():
	Sends fseek to the position of first byte of the previous page to read the previous blocl. The current page position is found using currPagePos in the file handler. 
	if previous page number is less than zero, the function returns the RC_READ_NON_EXISTING_PAGE error
	
 (9). readCurrentBlock ():
 	Fetches the current position curPagePos and reads the block.

 (10). readNextBlock ():
	function reads the next block by 
	Sends fseek to the position of first byte on the next page and reads the next block. 
	The current page position is found using currPagePos in the file handler. 
 	Returns RC_READ_NON_EXISTING_PAGE error if the previous page number is greater than the existing number of pages.

 (11). readLastBlock ():
 	- Pointer points the last block, which is totalNumPages-1 and reads the block.

 (12). writeBlock ():
	checks if a page number exists.
	If true, "fseek" seeks the file pointer until that page, and writes data from the page handler to the location using "fwrite".
	Returns the "NON-EXISTING PAGE" error if the page does not exist.


 (13). writeCurrentBlock ():
	Uses "fseek" to seek the file pointer until a page. The pointer is then incremented, and data from the page handler is written to the seeked location using fwrite.

 (14). appendEmptyBlock ():
	uses "fseek" to seek the pointer to the end, and adds a new block by calling "fputc()".
	Then, it increments the total number of pages available in the File Handler
	
 (15). ensureCapacity ():
	Checks the capacity and the number of existing pages. If less than the capacity, runs a process like "appendEmptyBlock()" to reach the capacity


------------------------- METADATA -------------------------

The metadata is used to store the number of pages in a file. 
The first few bytes of the pagefile (defined by sizeof(datatype)) are used to store the number of pages in the current file. In this case, it is sizeof(int).
This value is initialized when the file is created, and will be updated later on whenever changes are made. 
Page numbers are updated in the function, "closePageFile()"