#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<math.h>
#include "storage_mgr.h"

FILE *file;
void initStorageManager (void) {
	file = NULL;
	printf("Initializing storage manager\n");
}

void closefile(FILE *file) {
	fclose(file);
}

RC createPageFile (char *fileName) {
	if(fopen(fileName, "w+") != NULL)
	{
		file = fopen(fileName, "w+");
		SM_PageHandle newpage = (SM_PageHandle)calloc(PAGE_SIZE, sizeof(char));
		if(fwrite(newpage, sizeof(char), PAGE_SIZE,file) < PAGE_SIZE) { printf("Failed to write the file"); }
		else { free(newpage);}
	} 
	else
	{
		return RC_FILE_NOT_FOUND;
	}
	closefile(file);
	return RC_OK;
}

RC openPageFile (char *fileName, SM_FileHandle *fHandle) {
	file = fopen(fileName, "r");
	if (file != NULL)
	{
		fHandle->fileName = fileName;
	fHandle->curPagePos = 0;
	struct stat fileInfo;
	if(fstat(fileno(file), &fileInfo) < 0)    
	return RC_ERROR;
	fHandle->totalNumPages = fileInfo.st_size/ PAGE_SIZE;
	}
	else
	{
		return RC_FILE_NOT_FOUND;
	}
	closefile(file);
	return RC_OK;
}

RC closePageFile (SM_FileHandle *fHandle) {
	if (file == NULL)
	{
		return -1;
	}
	else
	{
		file = NULL;
	}
	return RC_OK; 
}


RC destroyPageFile (char *fileName) {
	if(fopen(fileName, "r") != NULL) 
		{remove(fileName);} 
	else 
		{return RC_FILE_NOT_FOUND;}
	return RC_OK;
}

RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
	if (pageNum < 0) {return RC_READ_NON_EXISTING_PAGE;}
	if (pageNum > fHandle->totalNumPages) {return RC_READ_NON_EXISTING_PAGE;}
	file = fopen(fHandle->fileName, "r");
	if(file == NULL) {return RC_FILE_NOT_FOUND;}
	if(fseek(file, (pageNum * PAGE_SIZE), SEEK_SET) == 0) {
		if(fread(memPage, sizeof(char), PAGE_SIZE, file) < PAGE_SIZE)
			{return RC_ERROR;}
	} 
	else {return RC_READ_NON_EXISTING_PAGE;}
	
	fHandle->curPagePos = ftell(file); 
	closefile(file);
    return RC_OK;
}

int getBlockPos (SM_FileHandle *fHandle) {
	int position = fHandle->curPagePos;
	return position;
}

RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	file = fopen(fHandle->fileName, "r");
	if (file != NULL)
	{
		for(int i = 0; i < PAGE_SIZE; i++) 
	{char c = fgetc(file);
	if(feof(file)) {break;}
	else {memPage[i] = c;}
	}
	}
	else
	{
		return RC_FILE_NOT_FOUND;
	}
	closefile(file);
	return RC_OK;

}

RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	if (fHandle->curPagePos > PAGE_SIZE)
	{
		int ptr_start = (PAGE_SIZE * ((fHandle->curPagePos / PAGE_SIZE) - 2));
	file = fopen(fHandle->fileName, "r");
	if(file == NULL) {return RC_FILE_NOT_FOUND;}
	fseek(file, ptr_start, SEEK_SET);
	int i=0;
	for(i = 0; i < PAGE_SIZE; i++) {memPage[i] = fgetc(file);}
	fHandle->curPagePos = ftell(file);
	}
	else
	{
		return RC_READ_NON_EXISTING_PAGE;
	}
	closefile(file);
	return RC_OK;
}

RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	file = fopen(fHandle->fileName, "r");

	if (file != NULL)
		{
			int ptr_start = (PAGE_SIZE * ((fHandle->curPagePos / PAGE_SIZE) - 2));
	fseek(file, ptr_start, SEEK_SET);
	int i;
	for(i = 0; i < PAGE_SIZE; i++) {
	char c = fgetc(file);		
	if(feof(file)) {break;}
	memPage[i] = c;
	}
	fHandle->curPagePos = ftell(file);
		}
		else
		{
			return RC_FILE_NOT_FOUND;
		}
		closefile(file);
	return RC_OK;	
}

RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
	if (fHandle->curPagePos != PAGE_SIZE)
	{
	int ptr_start = (PAGE_SIZE * ((fHandle->curPagePos / PAGE_SIZE) - 2));
	file = fopen(fHandle->fileName, "r");
	if(file != NULL) {return RC_FILE_NOT_FOUND;}
	fseek(file, ptr_start, SEEK_SET);
	int i;
	for(i = 0; i < PAGE_SIZE; i++) {
	char c = fgetc(file);		
	if(feof(file)) {break;}
	memPage[i] = c;
	}
	fHandle->curPagePos = ftell(file); 
	}
	else
	{
		return RC_READ_NON_EXISTING_PAGE;
	}
	closefile(file);
	return RC_OK;
}

RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
	if (file != NULL)
	{
		int ptr_start = (fHandle->totalNumPages - 1) * PAGE_SIZE;
	fseek(file, ptr_start, SEEK_SET);
	int i;
	for(i = 0; i < PAGE_SIZE; i++) {
	char c = fgetc(file);		
	if(feof(file)) {break;}
	memPage[i] = c;
	}
	fHandle->curPagePos = ftell(file); 
	}
	else
	{
		return RC_FILE_NOT_FOUND;
	}
	closefile(file);
	return RC_OK;
}

RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
	if (pageNum < 0) {return RC_WRITE_FAILED;}
    if (pageNum > fHandle->totalNumPages) {return RC_WRITE_FAILED;}
	file = fopen(fHandle->fileName, "r+");
	if(file == NULL) {return RC_FILE_NOT_FOUND;}
	int ptr_start = pageNum * PAGE_SIZE;
	if(pageNum == 0) { 
	fseek(file, ptr_start, SEEK_SET);	
	int i;
	for(i = 0; i < PAGE_SIZE; i++) 
	{
	if(feof(file)) {appendEmptyBlock(fHandle);}
	fputc(memPage[i], file);
	}
	fHandle->curPagePos = ftell(file); 
	closefile(file);	
	} 
	else {	
	fHandle->curPagePos = ptr_start;
	closefile(file);
	writeCurrentBlock(fHandle, memPage);
	}
	return RC_OK;
}

RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	file = fopen(fHandle->fileName, "r+");
	if (file != NULL)
	{
		appendEmptyBlock(fHandle);
	fseek(file, fHandle->curPagePos, SEEK_SET);
	fwrite(memPage, sizeof(char), strlen(memPage), file);
	fHandle->curPagePos = ftell(file);
	
	}
	else
	{
		return RC_FILE_NOT_FOUND;
	}
	closefile(file);
	return RC_OK;
}

RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle) {
	file = fopen(fHandle->fileName, "a");

	if (file != NULL)
	{
		do
		{
			appendEmptyBlock(fHandle);
			closefile(file);
		} while (fHandle->totalNumPages < numberOfPages);
	}
	else
	{
		return RC_FILE_NOT_FOUND;
	}
	closefile(file);
	return RC_OK;
}

RC appendEmptyBlock (SM_FileHandle *fHandle) {
	SM_PageHandle emptyBlock = (SM_PageHandle)calloc(PAGE_SIZE, sizeof(char));

	if (fseek(file, 0, SEEK_END) != 0)
	{
		free(emptyBlock);
		return RC_WRITE_FAILED;
	}
	else
	{
		fwrite(emptyBlock, sizeof(char), PAGE_SIZE, file);
		free(emptyBlock);
		fHandle->totalNumPages++;
	}
			return RC_OK;
}

