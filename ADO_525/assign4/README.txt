CS 525 - Spring 2020 - Assignment 4 - BTree Manager - Group 11

------------------------- MAKEFILE -------------------------

To compile the codebase
   > make clean
   > make default
To run test cases
   > make run


------------------------- FUNCTION EXPLANATION -------------------------
------------------------- buffer_mgr.c -------------------------

searchTree
> searches the tree for a node

setTreePosition
> sets current position in the tree structure

getTreePosition
> returns current position in the tree

createBtreeByCapacity
> initialized and creates a tree with a specified capacity

getBtreePtrSize
> returns the size of the pointer to the tree

isPageLoadable
> returns true/false if the current page can be loaded or not

isBtreeReachable
> if the page can be loaded, it loads the required page in the tree

getUpdBufPointer
> returns pointer to the buffer

updBtreeChildPage
> updates the page associated to the current child node

updBtreeLeftAndRightChild
> updates pointers to the left and right child of the current tree node

addNodeToBtree
> creates a new node in the btree

getBtreeNode
> searches for a node in the btree and gets its location

identifyNode
> returns the pointer to the located node

fillMyBtree
> scans btree for all available nodes

initBtreeParams
> initializes a new btree

insKeyOnAvailPos
> inserts a key into a specified index of the tree

updrightTreeLeft
> updates right child of left node

updrightTreeRight
> updates right child of right node

insertKeyAndUpdBTree
> inserts a key into the btree and updates the whole tree

splitAndMergeParent
> handles restructuring of the tree on insertion/deletion

initIndexManager
> initializes a new index

shutdownIndexManager
> closes a specified index

openBtree
> opens/creates a specified btree

closeBtree
> closes a specified btree

unlinkBtree
> unlinks a btree

deleteBtree
> deletes a specified btree ad frees up the allocated space

getNumNodes
> returns the total number of nodes in the tree

getNumEntries
> returns the total number of entries in the tree

getKeyType
> returns the type of key in the btree

findKey
> searches for a key in the tree and returns the node if key is found

initNewTreeKeys
> initializes parameters for a new key

traceSpotAndPos
> sets position for the left child node of the  tree

copyParentNode
> copies and moves the parent node

copyRightNode
> copies and moves the right node

copyNewBTree
> copies and moves a node in the tree

insertKey
> inserts a key into the tree

deleteKey
> deletes a key from the tree

openTreeScan
> scans a tree

nextEntry
> finds the next entry in a tree

closeTreeScan
> closes the scan that is currently running and frees up the space allocated by the scan handler

printTree
> displays the nodes of the tree