CS 525 - Spring 2020 - Assignment 2 - Buffer Manager

------------------------- FILES INCLUDED IN THE ASSIGNMENT -------------------------
The codebase for assignment 2 includes the following files:

1. storage_mgr.c
2. storage_mgr.h
3. test_assign1_1.c
4. test_assign1_2.c
5. test_helper.h
6. dberror.c
7. dberror.h
8. Makefile
9. README text file
10. buffer_mgr.h
11. buffer_mgr.c
12. buffer_mgr_stat.h
13. buffer_mgr_stat.c


------------------------- MAKEFILE -------------------------

Execute any one of the below commands to run the file:
- make 
- make run1
- make run2

These commands will create the output files "test1" and "test2", which can then be executed using "./tcase1" and "./tcase2"

To clear the generated output files, use the command:"make clean"



------------------------- INFO ON EXECUTION AND TEST CASES -------------------------

Memory management has been ensured, and reserved space is always freed up when available.
The following page replacement algorithms have been implemented:
 - FIFO
 - LRU

We have added additional test cases in the test case source file (test_assign2_2.c) to test the LFU and CLOCK page replacement strategies

------------------------- buffer_mgr.c -------------------------
-- Functions for Buffer Pool --
These functions create a buffer pool for a file on the disk. The storage manager from the first assignment will be used for managing page files.

initBufferPool()
 - initializes a new buffer pool in the memory. The numPages parameter determins the size of the buffer (number of pages that can be stored in the buffer). pageFileName stores the name of the pagefile being cached. strategy sets the page replacement strategy currently in use by the buffer pool. stratData contains arguments passed for the specific page replacement strategy.


shutdownBufferPool()
 - destroys / shuts down the buffer pool. All resources currently in use by the buffer manager are freed up.
 - this also calls the function forceFlushPool() that writes all modified (dirty) pages to the disk
 - will throw the RC_PINNED_PAGES_IN_BUFFER error if the page is in use by a client.

forceFlushPool()
 - all modified pages are written to the disk, wherever the dirty bit flag is marked as 1, as long as the page is not currently in use.



-- Functions for Page Management --
These functions load/remove pages from the buffer pool, marks pages as dirty, and forces a page to be written to the disk

pinPage()
 - reads a page from the pagefile and stores it in the buffer pool, if empty space is available in the buffer. If not available, a page replacement algorithm is used to determine which of the existing pages are to be removed, and then replaces an existing page in the pool.
 - this will write the replaced page to the disk if the dirty flag is marked as 1 (i.e. the page is modified)

unpinPage()
 - unpins a page from the buffer pool. It also marks whether the page is in use by a client or not.

makeDirty()
 - sets the dirty flag as 1 to mark a page as modified. 

forcePage()
 - the page content is written to the pagefile on the disk. It uses functionality from the storage manager implementation to perform these tasks, and sets dirty flag to 0 after the page is written.


-- Functions for Statistics --
These functions gathers statistical info about the buffer pool. 

getFrameContents()
 - gets an array of page numbers in the buffer pool.

getDirtyFlags()
 - returns a boolean list of dirty flags from the buffer pool.
  - modified pages have the flag as TRUE 

getFixCounts() 
- returns the fix count value of the page frames that are currently present in the buffer pool

getNumReadIO()
 - returns the total number of IO reads performed by the buffer pool

getNumWriteIO()
 - returns the total number of IO writes performed by the buffer pool


-- Functions for Page repacement algorithms --
4 page replacement strategies are implemented: FIFO, LRU, LFU, and CLOCK. These are used to determine which pages have to be removed when no free space is available on the buffer pool

FIFO()
 - this uses a very basic strategy: the first page that was added to the  buffer is removed when no space is available for new pages.

LRU()
 - this removes the page frame that hasn't been used for the longest duration. (least number of hits)