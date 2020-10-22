#include "dt.h"
#include "dberror.h"
#include "dberror.h"
#include "tables.h"

typedef struct SubBtreeBranch {
  int *branchObject;
  int branchStrVal;
  int branchLen;
} SubBtreeBranch;

typedef struct MainBTreeManager {
  struct MainBTreeManager *parBranch;
  struct MainBTreeManager *rtBranch;

  SubBtreeBranch *mbmKey;
  struct MainBTreeManager *ltBranch;
  SubBtreeBranch *leftChildTree;
  int mbmPgId;
  SubBtreeBranch *leftChildPosition;
  int mbmCapacity;
  SubBtreeBranch *childTree;
  int mbmLastNode;

  struct MainBTreeManager **nextBranchNode;
} MainBTreeManager;

typedef struct BtreeSearchInformation {
  int curTreeLoc;
  MainBTreeManager *currSubTree;
} BtreeSearchInformation;