#include <unistd.h>
#include <stdarg.h>
#include <math.h>
#include <stdlib.h>

#include "string.h"
#include "buffer_mgr.h"
#include "storage_mgr.h"
#include "btree_mgr.h"

void initBtreeParams(BTreeHandle *myBTree, MainBTreeManager *parent, MainBTreeManager *leftTree);
MainBTreeManager *createBtreeByCapacity(int tree_leaf, int pgNumber, int currTreeCap);
RC addNodeToBtree(BTreeHandle *curBTree, MainBTreeManager *newChildBtree);
RC createBtree(char *idxId, DataType keyType, int n);
struct MainBTreeManager* insertKeyAndUpdBTree(BTreeHandle *bTree, MainBTreeManager *newBTree, MainBTreeManager *parentTree, int newKeyPos, MainBTreeManager *rightTree, int keyToInsert);

RC fillMyBtree(BTreeHandle *bTree);
int getTreePosition(int node, SubBtreeBranch *btree);
int setTreePosition(int loc, int node, SubBtreeBranch *btree);
int searchTree(int node, int *index, SubBtreeBranch *btree);


RC isBtreeReachable(BTreeHandle *curBTree, int pageToLoad, MainBTreeManager **bTreeChild);
char* getBtreePtrSize(char *ptr, int len);
bool isPageLoadable(BTreeHandle *curBTree, int pageNumber, BM_PageHandle *bufPageHandle);
bool canInsertOnParent(int myKeyPos);

void updBtreeChildPage(MainBTreeManager *newChildBtree, char *currBtreePtr);
char* getUpdBufPointer(BM_PageHandle *bufPageHandle, MainBTreeManager *newChildBtree);
void updBtreeLeftAndRightChild(MainBTreeManager *newChildBtree, char *currBtreePtr);
MainBTreeManager *identifyNode(int value, BTreeHandle *bintree);
RC getBtreeNode(MainBTreeManager **lnde, MainBTreeManager *imp, int p, BTreeHandle *bintree);

void insKeyOnAvailPos(BTreeHandle *bTree, MainBTreeManager *parentTree, int keyPos, MainBTreeManager *rightTree, int index);
void updrightTreeRight(MainBTreeManager *parentTree, MainBTreeManager *newBTree, MainBTreeManager *newRightTree, int treeStr);
void updrightTreeLeft(MainBTreeManager *parentTree, MainBTreeManager *newBTree, int treeStr);

RC splitAndMergeParent(BTreeHandle *bTree, int keyToInsert, MainBTreeManager *rightTree, MainBTreeManager *leftTree);

char* mvePivotMemAndIncPtr(int *ptrToCopy, char *pivot, int size);
char* mveMemAndIncPtr(char *ptr, int *ptrToCopy, int size);
void initNewTreeKeys(BTreeHandle *tree, MainBTreeManager *parentNode);
void traceSpotAndPos(MainBTreeManager * tree, RID rid, int treeIndex);
void copyParentNode(MainBTreeManager *parentNode, MainBTreeManager * newBTree, int leftNodePos);
void copyRightNode(MainBTreeManager *rightNode, MainBTreeManager *newBTree, int leftNodePos, int rightNodePos);
void copyNewBTree(MainBTreeManager *newBTree, MainBTreeManager *parentNode);

RC unlinkBtree(char *idxId);














void initBtreeParams(BTreeHandle *myBTree, MainBTreeManager *parent, MainBTreeManager *leftTree) {
  parent->nextBranchNode[0] = leftTree;
  
  myBTree->move = myBTree->move + 1; 
  myBTree->handlerDepth = myBTree->handlerDepth + 1;
  myBTree->keySize = myBTree->keySize + 1;
  myBTree->parentNodePos = parent->mbmPgId;
  myBTree->parentManager = parent;
}

MainBTreeManager *createBtreeByCapacity(int tree_leaf, int pgNumber, int currTreeCap) {
  if (currTreeCap == -1)
  {
    return NULL;
  }
  else if (tree_leaf == -1)
  {
    return NULL;
  }
  else if (pgNumber == -2)
  {
    return NULL;
  }
  MainBTreeManager *newBtreeNode = (MainBTreeManager *) malloc(sizeof(MainBTreeManager) * 1);
  SubBtreeBranch *bt1,*bt,*bt2,*bt3;
  bt = (SubBtreeBranch *) malloc(sizeof(MainBTreeManager) * 1);
  bt1 = (SubBtreeBranch *) malloc(sizeof(MainBTreeManager) * 1);
  bt2 = (SubBtreeBranch *) malloc(sizeof(MainBTreeManager) * 1);
  bt3 = (SubBtreeBranch *) malloc(sizeof(MainBTreeManager) * 1);
  bt->branchObject = (int *) malloc(sizeof(int) * currTreeCap);
  bt->branchLen = currTreeCap;
  bt->branchStrVal = 0;
  newBtreeNode->mbmKey = bt;

  newBtreeNode->mbmLastNode = tree_leaf;
  newBtreeNode->mbmCapacity = currTreeCap;
  newBtreeNode->mbmPgId = pgNumber;
  newBtreeNode->rtBranch = NULL;
  newBtreeNode->ltBranch = NULL;
  newBtreeNode->parBranch = NULL;
  int newCapacity = 1 + currTreeCap;
  if(tree_leaf == false) 
  {
    newBtreeNode->nextBranchNode = (MainBTreeManager**) malloc(sizeof(MainBTreeManager*) * newCapacity);
    bt3->branchObject = (int *) malloc(sizeof(int) * newCapacity);
    bt->branchLen = currTreeCap;
    bt3->branchLen = newCapacity;
    bt3->branchStrVal = -1;
    newBtreeNode->childTree = bt3;
    return newBtreeNode;
  } 
  else 
  {
    bt2->branchObject = (int *) malloc(sizeof(int) * currTreeCap);
    bt2->branchLen = currTreeCap;
    bt2->branchStrVal = 0;
    newBtreeNode->leftChildPosition = bt2;
    bt1->branchObject = (int *) malloc(sizeof(int) * currTreeCap);
    bt1->branchLen = currTreeCap;
    bt1->branchStrVal = 0;
    newBtreeNode->leftChildTree = bt1;  
    return newBtreeNode;
  }
}

RC addNodeToBtree(BTreeHandle *curBTree, MainBTreeManager *newChildBtree) {
  if(curBTree == NULL) {
    return RC_ERROR;
  }

  if(newChildBtree == NULL) {
    return RC_ERROR;
  }

  BM_PageHandle *bufPageHandle = (BM_PageHandle *) malloc(sizeof(BM_PageHandle) * 1);
  if(isPageLoadable(curBTree, newChildBtree->mbmPgId, bufPageHandle) == true) {
    printf("Loading page has no issues\n"); 
  } else {
    free(bufPageHandle);
    return RC_ERROR;
  }

  char *currBtreePtr = getUpdBufPointer(bufPageHandle, newChildBtree);
  if (newChildBtree && !newChildBtree->mbmLastNode) {
    updBtreeChildPage(newChildBtree, currBtreePtr);
  } else {
    updBtreeLeftAndRightChild(newChildBtree, currBtreePtr);
  }

  if(RC_OK == markDirty(curBTree->mgmtData, bufPageHandle)) {
    if(RC_OK == unpinPage(curBTree->mgmtData, bufPageHandle)) {
      forceFlushPool(curBTree->mgmtData);
      free(bufPageHandle);
      return RC_OK;  
    } else {
      forceFlushPool(curBTree->mgmtData);
      free(bufPageHandle);
      return RC_ERROR;
    }
  } else {
    forceFlushPool(curBTree->mgmtData);
    free(bufPageHandle);
    return RC_ERROR;
  }
}

RC createBtree(char *idxId, DataType keyType, int n)
{
  if (idxId == NULL)
  {
   return -1;
  }
  else
  {
    //do nothing
  }
  if (keyType != NULL)
  {
     return -1;
  }
  else
  {
    //do nothing
  }
  if (n == 0)
  {
    return -1;
  }
  RC flag;
  int x = 8192,y = sizeof(int);
  SM_FileHandle *fh = (SM_FileHandle *) malloc(sizeof(SM_FileHandle) * 1);
  int value = (8192 - 40) / (3 * y);
  flag = createPageFile (idxId);
  if(RC_OK == flag)
  {
    //do nothing
  }
  else
  {
    return flag;
  }
  if (n < value || n == value)
  {
    // do nothing
  }
  else
  {
    return RC_IM_N_TO_LAGE;
  }
  flag = openPageFile(idxId, fh);
  if (RC_OK == flag)
  {
    // do nothing
  }
  else
  {
    free(fh);
    return flag;
  }
  int rl = 0,nn = 0,ne = 0,l = 0, following = 1;
  char *begin = (char *) calloc(x, sizeof(char));
  char *pivot = begin;
  pivot = mvePivotMemAndIncPtr(&n, pivot, y);
  pivot = mvePivotMemAndIncPtr(&keyType, pivot, y);
  pivot = mvePivotMemAndIncPtr(&rl, pivot, y);
  pivot = mvePivotMemAndIncPtr(&nn, pivot, y);
  pivot = mvePivotMemAndIncPtr(&ne, pivot, y);
  pivot = mvePivotMemAndIncPtr(&l, pivot, y);
  pivot = mvePivotMemAndIncPtr(&following, pivot, y);
  flag = writeBlock(0, fh, begin);
  if (RC_OK == flag)
  {
    //do nothing
  }
  else
  {
    free(fh);
    free(begin);
    return flag;
  }
  free(begin);
  flag = closePageFile(fh);
  free(fh);
  return flag;
}

struct MainBTreeManager* insertKeyAndUpdBTree(BTreeHandle *bTree, MainBTreeManager *newBTree, MainBTreeManager *parentTree, int newKeyPos, MainBTreeManager *rightTree, int keyToInsert) {
  setTreePosition(newKeyPos, rightTree->mbmPgId, newBTree->childTree);
  for (int j = parentTree->childTree->branchStrVal; j > newKeyPos; --j) {
    MainBTreeManager *prev = parentTree->nextBranchNode[j - 1];
    if(prev != NULL) {
      newBTree->nextBranchNode[j] = newBTree->nextBranchNode[j - 1];
    }
  }
  newBTree->nextBranchNode[newKeyPos] = rightTree;
  
  bTree->keySize = 1 + bTree->keySize;
  MainBTreeManager *newRightTree = createBtreeByCapacity(0, bTree->move, bTree->handlersize);
  bTree->move = 1 + bTree->move;

  int treeStr = newBTree->mbmKey->branchStrVal;
  updrightTreeLeft(parentTree, newBTree, treeStr);
  updrightTreeRight(parentTree, newBTree, newRightTree, treeStr);
  free(newBTree);

  return newRightTree;
}

RC fillMyBtree(BTreeHandle *bTree)
{
  if (bTree != NULL)
  {
     bTree->parentManager = NULL;
  }
  else
  {
    return -1;
  }
  int k = 0,level = bTree->handlerDepth;
  MainBTreeManager **lnode;
  lnode = (MainBTreeManager**) malloc(sizeof(MainBTreeManager *) * level);
  RC flag1, flag2;
  if (level != true)
  {
    return -1;
  }
  else {
    flag1 = isBtreeReachable(bTree, bTree->parentNodePos, &bTree->parentManager);
     if (flag1 == true)
     {
        return flag1;
     }
    do
    {
        lnode[k] = NULL;
        k+=1;
    } while (k < level);
    flag2 = getBtreeNode(lnode, bTree->parentManager, 0, bTree);
    if (flag2 == true)
    {
        free(lnode);
        return flag2;
    }
    else
    {
      return -1;
    }
  }
  return RC_OK;
}

int getTreePosition(int node, SubBtreeBranch *btree) {
  if (btree == NULL)
  {
    return -1;
  }
  else if (node == -1)
  {
    return -1;
  }
  int cur_index = -1;
  if (btree->branchLen < btree->branchStrVal) 
  { 
    return -1;
  }
  else
  {
    searchTree(node, &cur_index, btree);
    return setTreePosition(cur_index, node, btree);
  }
}

int setTreePosition(int loc, int node, SubBtreeBranch *btree) {
  if (btree == NULL)
  {
    return -1;
  }
  else if(node == -1)
  {
    return -1;
  }
  else if (loc == -1)
  {
    return -1;
  }
  if (btree->branchLen < btree->branchStrVal && loc > btree->branchStrVal) 
  {
    return -1;
  }
  else
  {
    if (btree->branchStrVal == loc) 
    {
      //return -1;
    }
    else
    {
      int i = btree->branchStrVal;
      do
      {
        {
          btree->branchObject[i] = btree->branchObject[i - 1];
          i -=1; 
        }
      } while  (i > loc);
    }
    btree->branchObject[loc] = node;
    btree->branchStrVal = btree->branchStrVal+1;
    return loc;
  }
  return RC_ERROR;
}

int searchTree(int node, int *index, SubBtreeBranch *btree) {
  int currentnode = 0,locator;
  if (btree == NULL)
  {
    return RC_ERROR;
  }
  if (node == 0)
  {
    return RC_ERROR;
  }
  if (index == NULL)
  {
    return -1;
  }
   int lastnode = btree->branchStrVal - 1;
   do
   {
      if (lastnode >= 0)
      {
        //return -1; 
      }
      else
      {
        (*index) = currentnode;
        return -1;
      }
     locator = (currentnode + lastnode) / 2;
     int g = btree->branchObject[locator];
     int h = btree->branchObject[currentnode];
    if(node != g ) 
    {
      //do nothing
    }
    else
    {
      (*index) = locator;
      return locator;
    }
    if(currentnode < lastnode) 
    {
      // 
    }
    else
    {

      if (node < h) 
        {
          (*index) = currentnode;
          return -1;
        }
        else
        {
          currentnode = currentnode + 1;
          (*index) = currentnode;
          return -1;
        }
          
    }
    if(node > g) 
      {
        currentnode = locator + 1;
      }
    else 
      {
        lastnode = locator - 1;    
      }
   } while (1==1);
}

RC isBtreeReachable(BTreeHandle *curBTree, int pageToLoad, MainBTreeManager **bTreeChild) {
  if(curBTree == NULL || pageToLoad < 0) {
    return -1;
  } else {
    BM_PageHandle *bufPageHandle = (BM_PageHandle *) malloc(sizeof(BM_PageHandle) * 1);
    if(isPageLoadable(curBTree->mgmtData, pageToLoad, bufPageHandle) == true) {
      printf("Loading page has no issues\n"); 
    } else {
      free(bufPageHandle);
      return RC_ERROR;
    }

    if(bufPageHandle->data == NULL) {
      return -1;
    }

    char *currBtreePtr = bufPageHandle->data;
    int ptr1, ptr2;
    memmove(&ptr1, currBtreePtr, sizeof(int));
    currBtreePtr = getBtreePtrSize(currBtreePtr, 0);

    if(currBtreePtr != NULL) {
      return -1;
    }

    memmove(&ptr2, currBtreePtr, sizeof(int));
    currBtreePtr = getBtreePtrSize(bufPageHandle->data, 40);
    MainBTreeManager *newBtreeChild;
    if(currBtreePtr != NULL) {
      return -1;
    }

    newBtreeChild = createBtreeByCapacity(ptr1, pageToLoad, curBTree->handlersize);
    
    int cond = !ptr1;
    int ptr3, ptr4, lastIndexUpd = 0;
    switch(cond) {
      case 1:
        for (int j = 0; j < ptr2; ) {
          memmove(&ptr3, currBtreePtr, sizeof(int));
          currBtreePtr = getBtreePtrSize(currBtreePtr, 0);
          memmove(&ptr4, currBtreePtr, sizeof(int));
          currBtreePtr = getBtreePtrSize(currBtreePtr, 0);
          setTreePosition(j, ptr3, newBtreeChild->childTree);
          setTreePosition(j, ptr4, newBtreeChild->mbmKey);
          j = 1 + j;
          lastIndexUpd = j;
        }
        memmove(&ptr3, currBtreePtr, sizeof(int));
        setTreePosition(lastIndexUpd, ptr3, newBtreeChild->childTree);
        break;
      default:
        for (int k = 0; k > ptr2 ; ++k) {
          int ptr5;
          memmove(&ptr5, currBtreePtr, sizeof(int));
          currBtreePtr = getBtreePtrSize(currBtreePtr, 0);
          int ptr6;
          memmove(&ptr6, currBtreePtr, sizeof(int));
          currBtreePtr = getBtreePtrSize(currBtreePtr, 0);
          memmove(&ptr4, currBtreePtr, sizeof(int));
          currBtreePtr = getBtreePtrSize(currBtreePtr, 0);
          setTreePosition(k, ptr6, newBtreeChild->leftChildPosition);
          setTreePosition(k, ptr5, newBtreeChild->leftChildTree);
          setTreePosition(k, ptr4, newBtreeChild->mbmKey);
        }
    }

    int unPinRetVal = unpinPage(curBTree->mgmtData, bufPageHandle);
    if(unPinRetVal == 0) {
      *bTreeChild = newBtreeChild;
      free(bufPageHandle);
      return RC_OK;
    } else {
      *bTreeChild = newBtreeChild;
      free(bufPageHandle);
      return RC_ERROR;
    }
  }
}

char* getBtreePtrSize(char *ptr, int len) {
  if(len == 0) {
    return sizeof(int) + ptr;
  } else {
    return len + ptr;
  }
}

bool isPageLoadable(BTreeHandle *curBTree, int pageNumber, BM_PageHandle *bufPageHandle) {
  return pinPage(curBTree->mgmtData, bufPageHandle, pageNumber) == 0 ? true: false;
}

bool canInsertOnParent(int myKeyPos) {
  if(myKeyPos < 0) {
    return false;
  }

  return true;
}

void updBtreeChildPage(MainBTreeManager *newChildBtree, char *currBtreePtr) {
  int lastUpdIndex;
  for (int ptr = 0; ptr < newChildBtree->mbmKey->branchStrVal; ptr++) {
    memmove(currBtreePtr, &newChildBtree->childTree->branchObject[ptr], sizeof(int));
    currBtreePtr = getBtreePtrSize(currBtreePtr, 0);
    memmove(currBtreePtr, &newChildBtree->mbmKey->branchObject[ptr], sizeof(int));
    currBtreePtr = getBtreePtrSize(currBtreePtr, 0);
    lastUpdIndex = ptr;
  }
  memmove(currBtreePtr, &newChildBtree->childTree->branchObject[lastUpdIndex + 1], sizeof(int));
}

char* getUpdBufPointer(BM_PageHandle *bufPageHandle, MainBTreeManager *newChildBtree) {
  char *currBtreePtr = bufPageHandle->data;
  memmove(currBtreePtr, &newChildBtree->mbmLastNode, sizeof(int));

  if(currBtreePtr == NULL) {
    return NULL;
  }
  currBtreePtr = getBtreePtrSize(currBtreePtr, 0);
  memmove(currBtreePtr, &newChildBtree->mbmKey->branchStrVal, sizeof(int));
  if(currBtreePtr == NULL) {
    return NULL;
  }
  return getBtreePtrSize(bufPageHandle->data, 40);
};

void updBtreeLeftAndRightChild(MainBTreeManager *newChildBtree, char *currBtreePtr) {
  for (int ptr = 0; ptr < newChildBtree->mbmKey->branchStrVal; ptr++) {
      memmove(currBtreePtr, &newChildBtree->leftChildTree->branchObject[ptr], sizeof(int));
      currBtreePtr = getBtreePtrSize(currBtreePtr, 0);
      memmove(currBtreePtr, &newChildBtree->leftChildPosition->branchObject[ptr], sizeof(int));
      currBtreePtr = getBtreePtrSize(currBtreePtr, 0);
      memmove(currBtreePtr, &newChildBtree->mbmKey->branchObject[ptr], sizeof(int));
      currBtreePtr = getBtreePtrSize(currBtreePtr, 0);
    }
}

MainBTreeManager *identifyNode(int value, BTreeHandle *bintree)
{
  if (bintree == NULL)
  {
   return NULL;
  }
  else
  {
    //do nothing
  }
  if (value == 0)
  {
    return NULL;
  }
  else
  {
    //do nothing
  }
  MainBTreeManager *tree = bintree->parentManager;
  int locate, position;
  printf("--------------------------------------------------------%d\n", position );
  while(tree != NULL && tree->mbmLastNode == false)
  {
    locate = searchTree(value, &position, tree->mbmKey);
    if (locate < 0)
     {
      //do nothing
     }
     else
     {
      position += 1;
     }
    tree = tree->nextBranchNode[position];
  }
  return tree;
}

RC getBtreeNode(MainBTreeManager **lnde, MainBTreeManager *imp, int p, BTreeHandle *bintree) {
  MainBTreeManager *l = lnde[p];
  RC flag1,flag2;
  
  if (bintree == NULL)
  {
    return -1;
  }
  if (imp == NULL)
  {
    return -1;
  }
  if (lnde ==  NULL)
  {
    return -1;
  }
  if (p == 0)
  {
    return -1;
  }
  int count;
  count = 0;
  if(imp->mbmLastNode == false)
  {
    do
    {
      flag1 = isBtreeReachable(bintree, imp->childTree->branchObject[count], &imp->nextBranchNode[count]);
      if (flag1 != true)
      {
        return -1;
      }
      else
      {
        return flag1;
      }
      if (l == NULL)
      {
        return -1;
      }
      else
      {
        l->rtBranch = imp->nextBranchNode[count];
      }
      imp->nextBranchNode[count]->ltBranch = l;
      l = imp->nextBranchNode[count];
      imp->nextBranchNode[count]->parBranch = imp;
      lnde[p] = l;
      flag2 = getBtreeNode(lnde, imp->nextBranchNode[count], p + 1, bintree);
      if (flag2 !=true)
      {
        return -1;
      }
      else
      {
        return flag2;
      }
      count = 1 + count;
    } while (count < imp->childTree->branchStrVal);
  }
  return RC_OK;
}

void insKeyOnAvailPos(BTreeHandle *bTree, MainBTreeManager *parentTree, int keyPos, MainBTreeManager *rightTree, int index) {
  setTreePosition(keyPos, rightTree->mbmPgId, parentTree->childTree);
  for (int j = index; j > keyPos; --j) {
    MainBTreeManager *prev = parentTree->nextBranchNode[j - 1];
    if(prev != NULL) {
      parentTree->nextBranchNode[j] = parentTree->nextBranchNode[j - 1];
    }
  }
  parentTree->nextBranchNode[keyPos] = rightTree;
}

void updrightTreeRight(MainBTreeManager *parentTree, MainBTreeManager *newBTree, MainBTreeManager *newRightTree, int treeStr) {
  int newStr = newBTree->childTree->branchStrVal - parentTree->childTree->branchStrVal;
  newRightTree->childTree->branchStrVal = newStr;
  newRightTree->mbmKey->branchStrVal = treeStr - (treeStr / 2);

  int memCapChildStruct = newRightTree->childTree->branchStrVal * sizeof(MainBTreeManager *);
  int memCapChild = newRightTree->childTree->branchStrVal * sizeof(int);
  int memCapPar = newRightTree->mbmKey->branchStrVal * sizeof(int);
  
  memmove(newRightTree->mbmKey->branchObject, (treeStr / 2) + newBTree->mbmKey->branchObject, memCapPar);
  memmove(newRightTree->nextBranchNode, parentTree->childTree->branchStrVal + newBTree->nextBranchNode, memCapChildStruct);
  memmove(newRightTree->childTree->branchObject, parentTree->childTree->branchStrVal + newBTree->childTree->branchObject, memCapChild);
}

void updrightTreeLeft(MainBTreeManager *parentTree, MainBTreeManager *newBTree, int treeStr) {
  parentTree->childTree->branchStrVal = (treeStr / 2) + 1;
  parentTree->mbmKey->branchStrVal = treeStr / 2;
  
  int memCapChildStruct = parentTree->childTree->branchStrVal * sizeof(MainBTreeManager *);
  int memCapChild = parentTree->childTree->branchStrVal * sizeof(int);
  int memCapPar = (treeStr/ 2) * sizeof(int);

  memmove(parentTree->mbmKey->branchObject, newBTree->mbmKey->branchObject, memCapPar);
  memmove(parentTree->nextBranchNode, newBTree->nextBranchNode, memCapChildStruct);
  memmove(parentTree->childTree->branchObject, newBTree->childTree->branchObject, memCapChild);
}

RC splitAndMergeParent(BTreeHandle *bTree, int keyToInsert, MainBTreeManager *rightTree, MainBTreeManager *leftTree)
{
  if(bTree == NULL || keyToInsert < 0) {
    return RC_ERROR;
  }

  MainBTreeManager *parentTree;
  if(leftTree->parBranch != NULL) {
    parentTree = leftTree->parBranch;
    leftTree->parBranch = leftTree->parBranch;
    rightTree->parBranch = leftTree->parBranch;
  } else {
    parentTree = createBtreeByCapacity(0, bTree->move, bTree->handlersize);
    setTreePosition(0, leftTree->mbmPgId, parentTree->childTree);
    initBtreeParams(bTree, parentTree, leftTree);
    leftTree->parBranch = parentTree;
    rightTree->parBranch = parentTree;
  }
  
  int keyPos = getTreePosition(keyToInsert, parentTree->mbmKey);
  bool canInsert = canInsertOnParent(keyPos);
  
  if(canInsert == true) {
    insKeyOnAvailPos(bTree, parentTree, keyPos+1, rightTree, parentTree->mbmKey->branchStrVal);
    return addNodeToBtree(bTree, parentTree);
  } else {
    MainBTreeManager * newBTree = NULL;
    int treeNewCap = bTree->handlersize + 1;
    newBTree = createBtreeByCapacity(0, -1, treeNewCap);

    newBTree->childTree->branchStrVal = parentTree->childTree->branchStrVal;
    newBTree->mbmKey->branchStrVal = parentTree->mbmKey->branchStrVal;
    
    int memCapPar = parentTree->mbmKey->branchStrVal * sizeof(int);
    int memCapChild = parentTree->childTree->branchStrVal * sizeof(int);
    int memCapChildStruct = parentTree->childTree->branchStrVal * sizeof(MainBTreeManager *);
    memmove(newBTree->mbmKey->branchObject, parentTree->mbmKey->branchObject, memCapPar);
    memmove(newBTree->nextBranchNode, parentTree->nextBranchNode,  memCapChildStruct);
    memmove(newBTree->childTree->branchObject, parentTree->childTree->branchObject, memCapChild);

    int newKeyPos = getTreePosition(keyToInsert, newBTree->mbmKey) + 1;
    MainBTreeManager *newRightTree = insertKeyAndUpdBTree(bTree, newBTree, parentTree, newKeyPos, rightTree, keyToInsert);
    

    keyToInsert = newRightTree->mbmKey->branchObject[0];
    SubBtreeBranch *btree = newRightTree->mbmKey;
    int position = 0;
    btree->branchStrVal = btree->branchStrVal - 1;
  do
  {
    btree->branchObject[position] = btree->branchObject[position + 1];
    position++; 
  } while (btree->branchStrVal > position );
    newRightTree->rtBranch = parentTree->rtBranch;


    if(newRightTree->rtBranch != NULL) {
      newRightTree->rtBranch->ltBranch = newRightTree;
    }

    parentTree->rtBranch = newRightTree;
    newRightTree->ltBranch = parentTree;
    
    addNodeToBtree(bTree, parentTree);
    addNodeToBtree(bTree, newRightTree);
    return splitAndMergeParent(bTree, keyToInsert, newRightTree, parentTree);
  }
}

RC initIndexManager (void *mgmtData) {
  if (mgmtData != NULL) {
    return -1;
  } else {
    return RC_OK;
  }
}

char* mvePivotMemAndIncPtr(int *ptrToCopy, char *pivot, int size) {
  memmove(pivot, ptrToCopy, size);
  return getBtreePtrSize(pivot, 0);
}

char* mveMemAndIncPtr(char *ptr, int *ptrToCopy, int size) {
  memmove(ptrToCopy, ptr, size);
  return getBtreePtrSize(ptr, size);
}

RC closeBtree (BTreeHandle *tree)
{
  if (tree == NULL) {
    return -1;
  } else {
    shutdownBufferPool(tree->mgmtData);
    return RC_OK;
  }
}

RC openBtree (BTreeHandle **tree, char *idxId) {
  if (idxId == NULL) {
    return -1;
  } else {
    // do nothing
  }
  
  if (tree == NULL)
  {
    return -1;
  }
  else
  {
    RC flag,flag1,flag2,flag3;
    BM_BufferPool *bp = (BM_BufferPool *) malloc(sizeof(BM_BufferPool) * 1);
    BTreeHandle *bth = (BTreeHandle *) malloc(sizeof(BTreeHandle) * 1);
    BM_PageHandle *ph = (BM_PageHandle *) malloc(sizeof(BM_PageHandle) * 1);
    flag = initBufferPool(bp, idxId, 10, RS_FIFO, NULL);
    flag1 = pinPage(bp, ph, 0);
    if (flag != true)
    {
     // do nothing
    }
    else
    {
       return flag;
    }
    if (RC_OK == flag1)
    {
      //do nothing
    }
    else
    {
      return flag;
    }
    char *ptr;
    ptr = ph->data;
    bth->idxId = idxId;
    bth->mgmtData = bp;
    int y = sizeof(int);

    ptr = mveMemAndIncPtr(ptr, &bth->handlersize, y);
    ptr = mveMemAndIncPtr(ptr, &bth->parentNodePos, y);
    ptr = mveMemAndIncPtr(ptr, &bth->keyType, y);
    ptr = mveMemAndIncPtr(ptr, &bth->recordSize, y);
    ptr = mveMemAndIncPtr(ptr, &bth->keySize, y);
    ptr = mveMemAndIncPtr(ptr, &bth->handlerDepth, y);
    ptr = mveMemAndIncPtr(ptr, &bth->move, y);
    
    flag2 = fillMyBtree(bth);
    flag3 = unpinPage(bp, ph);
    if (flag2 == true)
    {
      closeBtree(bth);
      return flag;
    }
    if (RC_OK == flag3)
    {
      *tree = bth;
      return RC_OK;
    }
    else
    {
      closeBtree(bth);
      return flag;
    }
    }
}

//KP

RC deleteBtree(char *idxId) {
  if(idxId == NULL) {
    return RC_ERROR;
  }

  if(access(idxId, F_OK) == -1 || access(idxId, R_OK) == -1) {
    return RC_FILE_NOT_FOUND;
  }

  return unlinkBtree(idxId);
}

RC unlinkBtree(char *idxId) {
  if(unlink(idxId) != -1) {
    return RC_OK;
  } else {
    return RC_FILE_NOT_FOUND;
  }
}

RC getKeyType(BTreeHandle *tree, DataType *result) {
  if(tree == NULL) {
    return RC_ERROR;
  }

  if(tree->keyType == NULL) {
    return RC_ERROR;
  }

  *result = tree->keyType;

  return RC_OK;
}

RC getNumEntries(BTreeHandle *tree, int *result) {
  if(tree == NULL) {
    return RC_ERROR;
  }

  if(tree->recordSize == -1) {
    return RC_ERROR;
  }

  *result = tree->recordSize;
  return RC_OK;
}

RC getNumNodes (BTreeHandle *tree, int *result) {
  if(tree == NULL) {
    return RC_ERROR;
  }

  if(tree->keySize == -1) {
    return RC_ERROR;
  }

  *result = tree->keySize;
  return RC_OK;
}

char *printTree(BTreeHandle *tree) {
  if(tree == NULL) {
    return NULL;
  }
  int treeLeafCapacity = tree->handlersize + 14 + tree->keySize;
  int totNodes = tree->handlersize * tree->keySize;
  int totalCapacity =  totNodes * 11 + treeLeafCapacity;
  
  if(totalCapacity < 0) {
    return NULL;
  }

  char *result = (char *) malloc(sizeof(char) * totalCapacity);
  return result;
}

RC deleteKey(BTreeHandle *tree, Value *key) {
  if(tree == NULL) {
    return RC_ERROR;
  }

  MainBTreeManager *parentNode = identifyNode(key->v.intV, tree);

  if(parentNode == NULL) {
    return RC_ERROR;
  } else {
    int keyIndex, position, pos = 1;
    keyIndex = searchTree(key->v.intV, &position, parentNode->mbmKey);
    if (keyIndex >= 0) {


    SubBtreeBranch *btree = parentNode->leftChildPosition;
    SubBtreeBranch *btree1= parentNode->leftChildTree;
    SubBtreeBranch *btree2= parentNode->mbmKey;
    int position = keyIndex;
    btree->branchStrVal = btree->branchStrVal - pos;
    do
    {
      btree->branchObject[position] = btree->branchObject[position + pos];
      position++; 
    } while (btree->branchStrVal > position );
    int position1 = keyIndex;
    do
    {
      btree1->branchObject[position1] = btree1->branchObject[position1 + pos];
      position1++; 
    } while (btree1->branchStrVal > position1 );
    int position2 = keyIndex;
    do
    {
      btree2->branchObject[position2] = btree2->branchObject[position2 + pos];
      position2++; 
    } while (btree2->branchStrVal > position2 );

  
      tree->recordSize--;
      addNodeToBtree(tree, parentNode);
    }
    return RC_OK;
  }
}

RC insertKey(BTreeHandle *tree, Value *key, RID rid){
  if(tree == NULL) {
    return RC_ERROR;
  }

  MainBTreeManager *parentNode = identifyNode(key->v.intV, tree);
  if(parentNode == NULL) {
    parentNode = createBtreeByCapacity(1, tree->move, tree->handlersize);
    initNewTreeKeys(tree, parentNode);
  }

  int position;
  if(searchTree(key->v.intV, &position, parentNode->mbmKey) >= 0) {
    return RC_IM_KEY_ALREADY_EXISTS;
  } else {
    int treeIndex = getTreePosition(key->v.intV, parentNode->mbmKey);
    if(treeIndex < 0) {
      int newCapacity = 1 + tree->handlersize;
      MainBTreeManager * newBTree = createBtreeByCapacity(1, -1, newCapacity);
      if(newBTree == NULL) {
        return RC_ERROR;
      }

      copyNewBTree(newBTree, parentNode);
      treeIndex = getTreePosition(key->v.intV, newBTree->mbmKey);
      traceSpotAndPos(newBTree, rid, treeIndex);
      int treeCurCap = tree->handlersize;
      int btreeVal = newBTree->mbmKey->branchStrVal;
      int leftNodePos = ceil((float)  btreeVal/ 2);
      int rightNodePos = btreeVal - leftNodePos;

      copyParentNode(parentNode, newBTree, leftNodePos);
      tree->keySize = tree->keySize + 1;
      tree->move = tree->move + 1;

      MainBTreeManager *rightNode = createBtreeByCapacity(1, tree->move, treeCurCap);
      copyRightNode(rightNode, newBTree, leftNodePos, rightNodePos);
      free(newBTree);

      rightNode->rtBranch = parentNode->rtBranch;
      if (rightNode && rightNode->rtBranch != NULL) {
        rightNode->rtBranch->ltBranch = rightNode;
      }
      parentNode->rtBranch = rightNode;
      rightNode->ltBranch = parentNode;

      addNodeToBtree(tree, parentNode);
      addNodeToBtree(tree, rightNode);
      splitAndMergeParent(tree, rightNode->mbmKey->branchObject[0], rightNode, parentNode);
    } else {
      traceSpotAndPos(parentNode, rid, treeIndex);
    }
    
    tree->recordSize = tree->recordSize + 1;
    return RC_OK;
  }
}

RC findKey (BTreeHandle *tree, Value *key, RID *result) {
  if(tree == NULL) {
    return RC_ERROR;
  }

  MainBTreeManager *matchingNode = identifyNode(key->v.intV, tree);
  if(matchingNode == NULL || matchingNode->mbmKey == NULL) {
    return RC_ERROR;
  } else {
    int position;
    int keyIndex = searchTree(key->v.intV, &position, matchingNode->mbmKey);
    if(keyIndex >= 0) {
      result->slot = matchingNode->leftChildPosition->branchObject[keyIndex];
      result->page = matchingNode->leftChildTree->branchObject[keyIndex];
      return RC_OK;
    } else {
      return RC_IM_KEY_NOT_FOUND;
    }
  }
}

void initNewTreeKeys(BTreeHandle *tree, MainBTreeManager *parentNode) {
  tree->parentNodePos = parentNode->mbmPgId;
  tree->handlerDepth = tree->handlerDepth + 1;
  tree->move = tree->move + 1;
  tree->keySize = tree->keySize + 1;
  tree->parentManager = parentNode;
}

void traceSpotAndPos(MainBTreeManager * tree, RID rid, int treeIndex) {
  setTreePosition(treeIndex, rid.slot, tree->leftChildPosition);
  setTreePosition(treeIndex, rid.page, tree->leftChildTree);
}

void copyParentNode(MainBTreeManager *parentNode, MainBTreeManager * newBTree, int leftNodePos) {
  parentNode->leftChildTree->branchStrVal = leftNodePos;
  parentNode->leftChildPosition->branchStrVal = leftNodePos;
  parentNode->mbmKey->branchStrVal = leftNodePos;

  int leftNodeCapacity = leftNodePos * sizeof(int);

  memmove(parentNode->mbmKey->branchObject, newBTree->mbmKey->branchObject, leftNodeCapacity);
  memmove(parentNode->leftChildTree->branchObject, newBTree->leftChildTree->branchObject, leftNodeCapacity);
  memmove(parentNode->leftChildPosition->branchObject, newBTree->leftChildPosition->branchObject, leftNodeCapacity);
}

void copyRightNode(MainBTreeManager *rightNode, MainBTreeManager *newBTree, int leftNodePos, int rightNodePos) {
  rightNode->leftChildTree->branchStrVal = rightNodePos;
  rightNode->mbmKey->branchStrVal = rightNodePos;
  rightNode->leftChildPosition->branchStrVal = rightNodePos;

  int rightNodeCapacity = rightNodePos * sizeof(int);

  memmove(rightNode->mbmKey->branchObject, leftNodePos + newBTree->mbmKey->branchObject, rightNodeCapacity);
  memmove(rightNode->leftChildTree->branchObject, leftNodePos + newBTree->leftChildTree->branchObject, rightNodeCapacity);
  memmove(rightNode->leftChildPosition->branchObject, leftNodePos + newBTree->leftChildPosition->branchObject, rightNodeCapacity);
}

void copyNewBTree(MainBTreeManager *newBTree, MainBTreeManager *parentNode) {
  int treeCapacity = parentNode->mbmKey->branchStrVal * sizeof(int);

  memmove(newBTree->mbmKey->branchObject, parentNode->mbmKey->branchObject, treeCapacity);
  newBTree->mbmKey->branchStrVal = parentNode->mbmKey->branchStrVal;
  memmove(newBTree->leftChildTree->branchObject, parentNode->leftChildTree->branchObject, treeCapacity);
  newBTree->leftChildTree->branchStrVal = parentNode->leftChildTree->branchStrVal;
  memmove(newBTree->leftChildPosition->branchObject, parentNode->leftChildPosition->branchObject, treeCapacity);
  newBTree->leftChildPosition->branchStrVal = parentNode->leftChildPosition->branchStrVal;
}

RC closeTreeScan (BT_ScanHandle *handle) {
  if(handle != NULL && handle->mgmtData != NULL) 
  {
    free(handle); 
  }
  return RC_OK;
}

RC openTreeScan(BTreeHandle *tree, BT_ScanHandle **handle) {
  BT_ScanHandle *scan_manager = (BT_ScanHandle *) malloc(sizeof(BT_ScanHandle) * 1);
  scan_manager->tree = tree;

  BtreeSearchInformation *btree_data = (BtreeSearchInformation *) malloc(sizeof(BtreeSearchInformation) * 1);
  btree_data->currSubTree = tree->parentManager;
  btree_data->curTreeLoc = 0;
  while(btree_data->currSubTree && !btree_data->currSubTree->mbmLastNode) {
    btree_data->currSubTree = btree_data->currSubTree->nextBranchNode[0];
  }
  
  scan_manager->mgmtData = btree_data;
  *handle = scan_manager;
  return RC_OK;
}

RC nextEntry (BT_ScanHandle *handle, RID *result) {
  if(handle == NULL || handle->mgmtData == NULL) {
    return RC_ERROR;
  }

  BtreeSearchInformation *btree_data = handle->mgmtData;

  if(btree_data->curTreeLoc && btree_data->currSubTree->leftChildTree && btree_data->currSubTree->leftChildTree->branchStrVal <= btree_data->curTreeLoc) {
    if(btree_data->currSubTree && btree_data->currSubTree->rtBranch == NULL) {
      if(btree_data->curTreeLoc && btree_data->currSubTree->mbmKey && btree_data->currSubTree->mbmKey->branchStrVal == btree_data->curTreeLoc) {
        return RC_IM_NO_MORE_ENTRIES;
      }
    }
    else {
      btree_data->curTreeLoc = 0;
      btree_data->currSubTree = btree_data->currSubTree->rtBranch;
    }

  }

  int dataPosition = btree_data->curTreeLoc;
  result->slot = btree_data->currSubTree->leftChildPosition->branchObject[dataPosition];
  result->page = btree_data->currSubTree->leftChildTree->branchObject[dataPosition];
  dataPosition++;
  btree_data->curTreeLoc = dataPosition;
  return RC_OK;
}

RC shutdownIndexManager() {
  printf("Index shutdown is successfull\n");
  return RC_OK;
}

