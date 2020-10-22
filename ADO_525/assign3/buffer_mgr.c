#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "buffer_mgr.h"
#include "storage_mgr.h"
#include <math.h>
#include <sys/time.h>

typedef struct Page
{
	int dirty; 
	int fixcount;
	SM_PageHandle data;
	PageNumber pageNum;
	long timestamp; 
} PageFrame;

long currenttime ()
{
	struct timeval time;
    gettimeofday(&time, NULL);
    long currtime = 1000000 * time.tv_sec + time.tv_usec;
    return currtime;
}

int bufferpool = 0, rpc = 0, wpc = 0;
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, 
		  const int numPages, ReplacementStrategy strategy, 
		  void *stratData)
{
	PageFrame *page = malloc(sizeof(PageFrame) * numPages);
	bufferpool = numPages;
	/*FILE *file;
	file = fopen(pageFileName,"r+");
	printf("%f*\n",file );
	if (file == NULL)
	{
		return -1;
	}*/
	if(numPages != 0)
	{
		for(int i = 0; i < bufferpool; i++)
		{
			page[i].dirty = 0;
			page[i].fixcount = 0;
			page[i].data = NULL;
			page[i].pageNum = NO_PAGE;
			page[i].timestamp = currenttime();
		}
		bm->pageFile = (char *)pageFileName;
		bm->numPages = bufferpool;
		bm->strategy = strategy;
		bm->mgmtData = page;
		wpc = 0;
		return RC_OK;
	}
	else
	{
		printf("Input is missing\n");
		return -1;
	}	
}

RC forceFlushPool(BM_BufferPool *const bm)
{
	if(bm == NULL || bm->mgmtData == NULL) {
		return -1;
	}

	PageFrame *page = (PageFrame *)bm->mgmtData;
	SM_FileHandle file;
	for(int i = 0;i<bufferpool;i++)
	{
		if(page[i].fixcount == 0)
		{
			if(page[i].dirty == 1)
			{
				openPageFile(bm->pageFile, &file);
				writeBlock(page[i].pageNum, &file, page[i].data);
				page[i].dirty = 0;
				wpc = wpc + 1;
				printf("forceFlushPool is done\n");
			}
		}
	}
	return RC_OK;
}

RC shutdownBufferPool(BM_BufferPool *const bm) {

	if(bm == NULL || bm->mgmtData == NULL) {
		return -1;
	}

	PageFrame *page = (PageFrame *)bm->mgmtData;
	forceFlushPool(bm);
	int goodtorelease = 0;
	
	for(int i = 0;i< bufferpool;i++) {
		if(page[i].dirty != 0 || page[i].fixcount != 0) {
			goodtorelease = 0;
			printf("Cannot release buffer as it contains page frames\n");
			return RC_OK;
		}
		else
		{
			goodtorelease = 1;
		}
	}
	if(goodtorelease == 1)
	{
		free(page);
		bm->mgmtData = NULL;
		printf("shutdownBufferPool is done\n");
		return RC_OK;
	}
	return -1;
}

RC markDirty(BM_BufferPool *const bm, BM_PageHandle *const page) {
	PageFrame *pf = (PageFrame *)bm->mgmtData;
	
	for(int i = 0;i<bufferpool;i++) {
		if(pf[i].pageNum == page->pageNum) {
			if(pf[i].dirty == 0) {
				pf[i].dirty = 1;
				printf("markDirty is done\n");
				return RC_OK;
			}
		}
	}
	return RC_ERROR;
}

RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page) {
	PageFrame *pf = (PageFrame *)bm->mgmtData;
	
	printf("unpinPage\n");
	int c = 0;
	for (int i = 0; i < bufferpool; ++i)
	{
		if (pf[i].pageNum == page->pageNum)
		{
			c++;
		}
	}
	if (c == 0)
	{
		return -1;
	}
	for(int i = 0;i<bufferpool;i++)
	{
		if(pf[i].pageNum == page->pageNum)
		{
			if(pf[i].fixcount != 0)
			{
				pf[i].fixcount = pf[i].fixcount-1;
				printf("unpinPage is done\n");
			}
		}
	}
	return RC_OK;
}

RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page) {
	PageFrame *pf = (PageFrame *)bm->mgmtData;
	
	for(int i = 0; i<bufferpool; i++)
	{
		if(pf[i].dirty == 1)
		{
			if(pf[i].fixcount == 0)
			{
				if(pf[i].pageNum == page->pageNum)
				{
					SM_FileHandle file;
					openPageFile(bm->pageFile, &file);
					writeBlock(pf[i].pageNum, &file, pf[i].data);
					pf[i].dirty = 0;
					printf("forcePage is done\n");
					wpc = wpc + 1;
					return RC_OK;
				}
			}
		}
	}
	return -1;
}

PageNumber *getFrameContents (BM_BufferPool *const bm) {
	PageNumber *frameContents = malloc(sizeof(PageNumber) * bufferpool);
	PageFrame *pageFrame = (PageFrame *) bm->mgmtData;
	
	for (int i = 0; i < bufferpool; ++i) {
		if(pageFrame[i].pageNum != -1) {
			frameContents[i] = pageFrame[i].pageNum;
		} else {
			frameContents[i] = -1;
		}
	}
	return frameContents;
}

bool *getDirtyFlags (BM_BufferPool *const bm) {
	bool *dirty_flags = malloc(sizeof(bool) * bufferpool);
	PageFrame *page = (PageFrame *)bm->mgmtData;
	
	for(int i = 0; i < bufferpool; i++) 
	{
		if(page[i].dirty == 1) 
		{
			dirty_flags[i] = true;
		} 
		else
		{
			dirty_flags[i] = false;
		}
	}
	return dirty_flags;
}

int *getFixCounts (BM_BufferPool *const bm) {
	int *fix_counts = malloc(sizeof(int) * bufferpool);
	PageFrame *page= (PageFrame *)bm->mgmtData;
	
	for(int i = 0; i < bufferpool; i++) 
	{
		if(page[i].pageNum != -1) 
		{
			fix_counts[i] = page[i].fixcount;
		}
		else
		{
			fix_counts[i] = 0;
		}
	}
	return fix_counts;
}

int getNumReadIO (BM_BufferPool *const bm) {
	int readIOCnt = 1 + rpc;
	return readIOCnt;
}

int getNumWriteIO (BM_BufferPool *const bm) {
	return wpc;
}

void FIFO(BM_BufferPool *const bm, PageFrame *page) {
	PageFrame *pageFrame = (PageFrame *) bm->mgmtData;
	int k = -1, frontIndex;
	long first = currenttime();
	printf("first : %ld\n", first);
	printf("page : %ld\n", pageFrame[0].timestamp);
	frontIndex = rpc % bufferpool;
	for (int i = 0; i < bufferpool; ++i)
	{
		printf("%ld\n",pageFrame[i].timestamp );
		if (pageFrame[i].timestamp < first && pageFrame[i].fixcount == 0) {
			first = pageFrame[i].timestamp;
			k = i;
		}
		else
			printf("nothing\n");
	}
	
	if(pageFrame[k].dirty == 1 && k !=-1) {
		SM_FileHandle fh;
		openPageFile(bm->pageFile, &fh);
		writeBlock(pageFrame[k].pageNum, &fh, pageFrame[k].data);
		wpc++;
	}

	pageFrame[k].timestamp = currenttime();
	pageFrame[k].fixcount = page->fixcount;
	pageFrame[k].dirty = page->dirty;
	pageFrame[k].pageNum = page->pageNum;
	pageFrame[k].data = page->data;
}

void LRU(BM_BufferPool *const bm, PageFrame *page) {
	PageFrame *pageFrame = (PageFrame *) bm->mgmtData;
	int k = -1;
	long first = currenttime();
	for (int i = 0; i < bufferpool; ++i) {
		if(pageFrame[i].timestamp < first && pageFrame[i].fixcount == 0) {
			first = pageFrame[i].timestamp;
			k = i;
		}
	}
	if(pageFrame[k].dirty == 1) {
		SM_FileHandle fh;
		openPageFile(bm->pageFile, &fh);
		writeBlock(pageFrame[k].pageNum, &fh, pageFrame[k].data);
		wpc++;
	}

	pageFrame[k].timestamp = currenttime();
	pageFrame[k].fixcount = page->fixcount;
	pageFrame[k].dirty = page->dirty;
	pageFrame[k].pageNum = page->pageNum;
	pageFrame[k].data = page->data;
}

int setvalues(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum, int i) {
    PageFrame *pf = (PageFrame *)bm->mgmtData;
	SM_FileHandle fh;
	openPageFile(bm->pageFile, &fh);
	pf[i].data = (SM_PageHandle) malloc(PAGE_SIZE);
	ensureCapacity(pageNum,&fh);
	readBlock(pageNum, &fh, pf[i].data);

	pf[i].timestamp = currenttime();
	pf[i].pageNum = pageNum;
	pf[i].fixcount++;
	
	page->pageNum = pageNum;
	page->data = pf[i].data;

	rpc = 0;
	return 1;
}
int createnewframe(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum);
int checkvalues(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum) {
	PageFrame *pf = (PageFrame *)bm->mgmtData;
	
	for(int i = 0; i < bufferpool; i++) {
		if(pf[i].pageNum == -1) {
			SM_FileHandle fh;
			openPageFile(bm->pageFile, &fh);
			pf[i].data = (SM_PageHandle) malloc(PAGE_SIZE);
			readBlock(pageNum, &fh, pf[i].data);
			
			pf[i].timestamp = currenttime();
			pf[i].fixcount = 1;
			pf[i].pageNum = pageNum;
			rpc++;
			page->data = pf[i].data;
			page->pageNum = pageNum;
			return 1;
		} else if(pf[i].pageNum == pageNum)
			{
				pf[i].timestamp = currenttime();
				pf[i].fixcount++;
				page->data = pf[i].data;
				page->pageNum = pageNum;
				
				return 1;
			}
	}	
int f = createnewframe(bm, page, pageNum);
	return f;
}

int createnewframe(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum) {
	PageFrame *newPageFrame = (PageFrame *) malloc(sizeof(PageFrame));
	SM_FileHandle fh;
	openPageFile(bm->pageFile, &fh);
	newPageFrame->data = (SM_PageHandle) malloc(PAGE_SIZE);
	readBlock(pageNum, &fh, newPageFrame->data);
	
	newPageFrame->timestamp = currenttime();
	newPageFrame->fixcount = 1;
	newPageFrame->dirty = 0;		
	newPageFrame->pageNum = pageNum;

	page->data = newPageFrame->data;
	page->pageNum = pageNum;

	switch(bm->strategy) {
		case RS_LRU:
			LRU(bm, newPageFrame);
			break;
		case RS_FIFO:
			FIFO(bm, newPageFrame);
			break;
		default:
			printf("Doing nothing for now \n");
			break;
	}

	rpc++;
	return 1;
}

RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum) {

	if(bm == NULL || bm->mgmtData == NULL) {
		return -1;
	}

	PageFrame *pf = (PageFrame *)bm->mgmtData;
	int l = 0,p = 0;
	for (int i = 0; i < bufferpool; ++i)
	{
		if(pf[i].fixcount > 0)
		{
			p++;
		}
		if (p == bufferpool)
		{
			return -1;
		}
	}
	if (pageNum < 0)
	{
		return -1;
	}
	if(pf[l].pageNum == NO_PAGE) {
		int rc = setvalues(bm, page, pageNum, l);
		if (rc == 1) {
			return RC_OK;
		}
	}
	else
	{
		int checg = checkvalues(bm, page, pageNum);
		if(checg == 1) {
			return RC_OK;
		}
	}
	return -1;
}