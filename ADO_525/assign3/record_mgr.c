#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "record_mgr.h"
#include "buffer_mgr.h"
#include "storage_mgr.h"



const int int_size = sizeof(int);

TableDataManager *tableDataManager;

//------------------------------------------------------------------------------------------------------------------
int findSlot(char *data, int recordSize);
int Ctable(char *name, Schema *schema);
int Otable(RM_TableData *rel, char *name);
int checkresult(int result);
void resetScanManager(RM_ScanHandle *scan);
//------------------------------------------------------------------------------------------------------------------

RC shutdownRecordManager () {
	if(tableDataManager != NULL) {tableDataManager = NULL; 
		free(tableDataManager);}
	return 0;
}

int checkresult(int result) {
	if(result != RC_OK) {return result;}
return -1;
}


RC initRecordManager (void *mgmtData) {
	if (mgmtData != NULL)
	{
		return -1;
	}
	else
	{
		initStorageManager();
		return RC_OK; 
	}
	
}

RC closeTable (RM_TableData *rel)
{
	if (rel == NULL)
	{
		return -1;
	}
	else
	{
		TableDataManager *dataManager = rel->mgmtData;
		if(dataManager != NULL) {shutdownBufferPool(&dataManager->bmManager); return RC_OK;}
		return RC_OK;
	}
	
}

int findSlot(char *data, int recordSize)
{
	int i=0;
	if (recordSize == 0)
	{
		return -1;
	}
	else if (data == NULL)
	{
		return -1;
	}
	int numslots = PAGE_SIZE/recordSize; 
	do
	{
		if (data[i * recordSize] != 'i') {return i;} i=i+1;
	} while (i < numslots);
	
	if(i==numslots) 
	{
		printf("\nNo free slots found.\n");
	return -1;}

	return RC_ERROR;
}


RC openTable (RM_TableData *rel, char *name)
{
	if (rel == NULL)
	{
		return -1;
	}
	else if (name == NULL)
	{
		return -1;
	}
	else
	{
		Otable(rel,name);
	}
	return RC_OK;
	}
	

RC deleteTable (char *name)
{	
	printf("closing pagefile\n");
	if(name != NULL)
	destroyPageFile(name);
	else
	{
		return -1;
	}
	return RC_OK;
}

int getNumTuples (RM_TableData *rel)
{
	if (rel == NULL)
	{
		return -1;
	}
	else
	{
		TableDataManager *dataManager = rel->mgmtData;
		if(dataManager != NULL) {int rowcount = dataManager->noOfTuples; return rowcount;}
		return -1;
	}
	
}

RC createTable (char *name, Schema *schema)
{
	if (name == NULL)
	{
		return -1;
	}
	else if (schema == NULL)
	{
		return -1;
	}
	else
	{
		Ctable(name,schema);
		return RC_OK;
	}
	
}


RC getRecord (RM_TableData *rel, RID id, Record *record) {
	if(rel == NULL || rel->mgmtData == NULL) {
		return -1;
	}

	if(record == NULL) {
		return -1;
	}

	pinPage(&((TableDataManager *) rel->mgmtData)->bmManager, &((TableDataManager *) rel->mgmtData)->bufferPageFileHandler, id.page);
	int slotSize = id.slot * getRecordSize(rel->schema);
	char *pageData = ((TableDataManager *) rel->mgmtData)->bufferPageFileHandler.data;
	pageData = pageData + slotSize;

	if(pageData == NULL || *pageData != 'i') {
		return RC_IM_KEY_NOT_FOUND;
	}

	record->id = id;
	char *recordData = record->data;
	memmove(++recordData, pageData+1, getRecordSize(rel->schema)-1);
	return RC_OK;
}


RC deleteRecord (RM_TableData *rel, RID id) {
	if(rel == NULL || rel->mgmtData == NULL) {
		return -1;
	}

	if(id.page == -1) {
		return -1;
	}
	 
	pinPage(&((TableDataManager *) rel->mgmtData)->bmManager, &((TableDataManager *) rel->mgmtData)->bufferPageFileHandler, id.page);
	((TableDataManager *) rel->mgmtData)->unallocatedPage = id.page;

	int slotSize = id.slot * getRecordSize(rel->schema);
	
	char *pageData = ((TableDataManager *) rel->mgmtData)->bufferPageFileHandler.data;
	
	pageData = pageData + slotSize;
	*pageData = 'd';
		
	markDirty(&((TableDataManager *) rel->mgmtData)->bmManager, &((TableDataManager *) rel->mgmtData)->bufferPageFileHandler);

	return RC_OK;
}

RC insertRecord (RM_TableData *rel, Record *record) {
	if(rel == NULL || rel->mgmtData == NULL) {
		return -1;
	}
	if(record == NULL) {
		return -1;
	}

	RID *newRecId = &record->id;

	newRecId->page = ((TableDataManager *) rel->mgmtData)->unallocatedPage;
	pinPage(&((TableDataManager *) rel->mgmtData)->bmManager, &((TableDataManager *) rel->mgmtData)->bufferPageFileHandler, newRecId->page);
	char *pageData = ((TableDataManager *) rel->mgmtData)->bufferPageFileHandler.data;
	newRecId->slot = findSlot(pageData, getRecordSize(rel->schema));

	for (int i = 0; newRecId->slot == -1; ++i) {
		newRecId->page++;
		pinPage(&((TableDataManager *) rel->mgmtData)->bmManager, &((TableDataManager *) rel->mgmtData)->bufferPageFileHandler, newRecId->page);
		pageData = ((TableDataManager *) rel->mgmtData)->bufferPageFileHandler.data;
		newRecId->slot = findSlot(pageData, getRecordSize(rel->schema));
	}

	char *slotted_data_pointer = pageData;
	markDirty(&((TableDataManager *) rel->mgmtData)->bmManager, &((TableDataManager *) rel->mgmtData)->bufferPageFileHandler);
	slotted_data_pointer = slotted_data_pointer + (newRecId->slot * getRecordSize(rel->schema));

	*slotted_data_pointer = 'i';

	memmove(++slotted_data_pointer, record->data + 1, getRecordSize(rel->schema) - 1);
	pinPage(&((TableDataManager *) rel->mgmtData)->bmManager, &((TableDataManager *) rel->mgmtData)->bufferPageFileHandler, 0);
	((TableDataManager *) rel->mgmtData)->noOfTuples++;

	return RC_OK;
}


RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond) {
	if(rel == NULL || rel->mgmtData == NULL) {
		return -1;
	}

	if(scan == NULL) {
		return -1;
	}

	if (cond == NULL) {
		return -1;
	}

	if(rel != NULL) {
		openTable(rel, "MyTable");	
	}

    TableDataManager * scanner = (TableDataManager*) malloc(sizeof(TableDataManager));

    scanner->constraint = cond;
    scanner->noOfScans = 0;
    scanner->recId.slot = 0;
    scanner->recId.page = 1;

    scan->mgmtData = scanner;

    ((TableDataManager *) rel->mgmtData)->noOfTuples = 15;
    scan->rel= rel;

	return RC_OK;
}

RC updateRecord (RM_TableData *rel, Record *record) {
	if(rel == NULL || rel->mgmtData == NULL) {
		return -1;
	}

	if(record == NULL) {
		return -1;
	}
	pinPage(&((TableDataManager *) rel->mgmtData)->bmManager, &((TableDataManager *) rel->mgmtData)->bufferPageFileHandler, record->id.page);
	int slotSize = record->id.slot * getRecordSize(rel->schema);

	char *pageData = ((TableDataManager *) rel->mgmtData)->bufferPageFileHandler.data;
	pageData = pageData + slotSize;
	*pageData = 'i';
	
	memmove(++pageData, record->data + 1, getRecordSize(rel->schema) - 1);
	markDirty(&((TableDataManager *) rel->mgmtData)->bmManager, &((TableDataManager *) rel->mgmtData)->bufferPageFileHandler);
	
	return RC_OK;	
}


RC closeScan (RM_ScanHandle *scan) {
	if(scan == NULL || scan->mgmtData == NULL) {
		return -1;
	}

	TableDataManager *scanner = scan->mgmtData;

	if(scanner->noOfScans != 0) {
		resetScanManager(scan);
	}
	
	if(scan->mgmtData != NULL) {
		scan->mgmtData = NULL;
		free(scan->mgmtData);
	}

	return RC_OK;
}

void resetScanManager(RM_ScanHandle *scan) {
	((TableDataManager *) scan->mgmtData)->recId.slot = 0;
	((TableDataManager *) scan->mgmtData)->recId.page = 1;
	((TableDataManager *) scan->mgmtData)->noOfScans = 0;
}

int a = sizeof(int),b = sizeof(float),c = sizeof(bool);
int getRecordSize(Schema *schema)
{
	int Length = 0,attr = schema->numAttr;

	if (schema != NULL)
	{	
		
		for (int i = 0; i < attr; ++i)
		{
			char datatype = schema->dataTypes[i];
			if(datatype == DT_INT)
			{
				Length +=  a;
			}
			else if (datatype == DT_FLOAT)
			{
				Length += b;
			}
			else if (datatype == DT_BOOL)
			{
				Length +=  c;
			}
			else if (datatype == DT_STRING)
			{
				int d = (sizeof(char) * schema->typeLength[i]);
				Length += d;
			}
			else
			{
				printf("+++++++++++++++++++++++Unknown datatype++++++++++++++++++++++++++\n");
				return -1;
			}
		}
	}
	else
	{
		return -1;
	}
	return Length;

}

int Ctable(char *name, Schema *schema)
{
		tableDataManager = (TableDataManager*) malloc(sizeof(TableDataManager));
		initBufferPool(&tableDataManager->bmManager, name, 100, RS_LRU, NULL);
		char data[PAGE_SIZE];
		char *filepagehandler = data;
		if(filepagehandler != NULL) {
		*(int*)filepagehandler = 0; 
		filepagehandler = filepagehandler + int_size;
		*(int*)filepagehandler = 1;
		filepagehandler = filepagehandler + int_size;
		*(int*)filepagehandler = schema->numAttr;
		filepagehandler = filepagehandler + int_size; 
		*(int*)filepagehandler = schema->keySize;
		filepagehandler = filepagehandler + int_size;
		}
		int i=0;
		do
		{
			memmove(filepagehandler, schema->attrNames[i], 15);
	       	filepagehandler = filepagehandler + 15;
	       	*(int*)filepagehandler = (int)schema->dataTypes[i];
	      	filepagehandler = filepagehandler + int_size;
	       	*(int*)filepagehandler = (int) schema->typeLength[i];
	       	filepagehandler = filepagehandler + int_size;
	       	i = i+1;
		} while (i < schema->numAttr);

		SM_FileHandle pagefilehandler;

		
		int value = createPageFile(name);
		if(value == checkresult(value)) {return value;}
		value = openPageFile(name, &pagefilehandler);
		if(value == checkresult(value)) {return value;}
		value = writeBlock(0, &pagefilehandler, data);
		if(value == checkresult(value)) {return value;}
		value = closePageFile(&pagefilehandler);
		if(value == checkresult(value)) {return value;}

		return 0;
		
}


RC next (RM_ScanHandle *scan, Record *record) {
	if(scan == NULL || scan->mgmtData == NULL || scan->rel == NULL || scan->rel->mgmtData == NULL) {
		return -1;
	}

	if(((TableDataManager *) scan->mgmtData)->constraint == NULL) {
		return -1;
	}

	if (record == NULL) {
		return -1;
	}

	if(((TableDataManager *) scan->rel->mgmtData)->noOfTuples == 0) {
		return RC_RM_NO_MORE_TUPLES;
	}

	Schema *tableSchema = scan->rel->schema;
	TableDataManager *scanner = scan->mgmtData;
	int scannerCnt = scanner->noOfScans;
	int tablesTuplesCnt = ((TableDataManager *)scan->rel->mgmtData)->noOfTuples;
	
	while(tablesTuplesCnt >= scannerCnt) {
		if(scannerCnt > 0) {
			if(scanner->recId.slot >= (PAGE_SIZE / getRecordSize(tableSchema))) {
				scanner->recId.page++;
				scanner->recId.slot = 0;
			} else {
				scanner->recId.slot++;
			}
		} else if (scannerCnt <= 0) {
			scanner->recId.slot = 0;
			scanner->recId.page = 1;
		}

		pinPage(&((TableDataManager *)scan->rel->mgmtData)->bmManager, &scanner->bufferPageFileHandler, scanner->recId.page);
		int slotSize = scanner->recId.slot * getRecordSize(tableSchema);
		char *pageData = scanner->bufferPageFileHandler.data;

		pageData = pageData + slotSize;

		record->id.page = scanner->recId.page;
		record->id.slot = scanner->recId.slot;

		char *recordData = record->data;
		*recordData = 'd';
		memmove(++recordData, pageData + 1, getRecordSize(tableSchema)-1);
		
		Value *resData = (Value *) malloc(sizeof(Value));
		evalExpr(record, tableSchema, scanner->constraint, &resData);

		scannerCnt++;
		scanner->noOfScans++;

		if(resData->v.boolV != FALSE) {
			return RC_OK;
		}
	}
	
	return RC_RM_NO_MORE_TUPLES;
}



RC freeSchema (Schema *schema)
{
	if (schema != NULL) {
		free(schema);
	} else {
		return -1;
	}
	return RC_OK;
}

int Otable(RM_TableData *rel, char *name)
 {
 	SM_PageHandle pagefilehandler;    
		rel->mgmtData = tableDataManager;
		rel->name = name;
		pinPage(&tableDataManager->bmManager, &tableDataManager->bufferPageFileHandler, 0);
		pagefilehandler = (char*) tableDataManager->bufferPageFileHandler.data;

		tableDataManager->noOfTuples= *(int*)pagefilehandler;
		pagefilehandler = pagefilehandler + int_size;
		tableDataManager->unallocatedPage= *(int*) pagefilehandler;
	   	pagefilehandler = pagefilehandler + int_size;
		int numattr = *(int*)pagefilehandler;
		pagefilehandler = pagefilehandler + int_size;

	 	Schema *tableschema = (Schema*) malloc(sizeof(Schema));
		tableschema->attrNames = (char**) malloc(sizeof(char*) *numattr);
		int i=0,j=0;
		do
		{
			tableschema->attrNames[i]= (char*) malloc(15);
			i= i+1;
		} while (i < numattr);
		tableschema->dataTypes = (DataType*) malloc(sizeof(DataType) *numattr);
		tableschema->typeLength = (int*) malloc(int_size *numattr);
		tableschema->numAttr = numattr;
		do
		{
			memmove(tableschema->attrNames[j], pagefilehandler, 15);
			pagefilehandler = pagefilehandler+15;
			tableschema->dataTypes[j]= *(int*) pagefilehandler;
			pagefilehandler = pagefilehandler+int_size;
			tableschema->typeLength[j]= *(int*)pagefilehandler;
			pagefilehandler = pagefilehandler+int_size;
			j = j+1;
		} while (j < tableschema->numAttr);
		rel->schema = tableschema;	
		return 0;
 }



Schema *createSchema (int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys) {
	
	Schema *createschema = (Schema *) malloc(sizeof(Schema));
	if (numAttr != 0)
	{
		createschema->numAttr = numAttr;
	} else {
		return NULL;
	}
	if (keySize != 0)
	{
		createschema->keySize = keySize;
	} else {
		return NULL;
	}
	if (dataTypes != NULL)
	{
		createschema->dataTypes = dataTypes;
	} else {
		return NULL;
	}
	if (keys != 0)
	{
		createschema->keyAttrs = keys;
	} else {
		return NULL;
	}
	if (attrNames != NULL)
	{
		createschema->attrNames = attrNames;
	} else {
		return NULL;
	}
	if (createschema != 0)
	{
		createschema->typeLength = typeLength;
	} else {
		return NULL;
	}
	
	
	return createschema; 
}

RC createRecord (Record **record, Schema *schema)
{
	if (schema != NULL)
	{
		int Length = getRecordSize(schema);
		Record *createrecord = (Record *) malloc(Length);
		createrecord->id.page = -1;
		createrecord->id.slot = -1;
		createrecord->data = (char*) malloc(Length);
		*record = createrecord;
		char *position = createrecord->data;
		*position = 'd';
		position = position + 1;
		*position = '\0';
	}
	else
	{
		return -1;
	}
	return RC_OK;
}



RC freeRecord (Record *record)
{
	if (record == NULL)
	{
		return -1;
	}
	else
	{
		free(record);
		return RC_OK;
	}
	
}

RC setAttr (Record *record, Schema *schema, int attrNum, Value *value)
{
	int balance1 = 0;
	int *balance = &balance1;
	 *balance = 1;
	 if (schema == NULL)
	 {
	 	return -1;
	 }
	 else
	 {
	 	for (int i = 0; i < attrNum; ++i)
		{
			if (schema->dataTypes[i] == DT_INT)
			{
				*balance = *balance+a;
			}
			else if (schema->dataTypes[i] == DT_FLOAT)
			{
				*balance = *balance+b;
			}
			else if (schema->dataTypes[i] == DT_BOOL)
			{
				*balance = *balance+c;
			}
			else if (schema->dataTypes[i] == DT_STRING)
			{
				*balance = *balance+schema->typeLength[i];
			}
			else
			{
				printf("--------------------------Unknown datatype------------------------------\n");
				return -1;
			}
		}
		char *data = record->data;
		data = data + balance1;

		if (schema->dataTypes[attrNum] == DT_INT)
		{
			*(int *)data = value->v.intV;	  
			data = data+a;
		}
		else if (schema->dataTypes[attrNum] == DT_FLOAT)
		{
			*(float *) data = value->v.floatV;
			data = data+b;
		}
		else if (schema->dataTypes[attrNum] == DT_BOOL)
		{
			*(bool *) data = value->v.boolV;
			data = data + sizeof(bool);
		}
		else if (schema->dataTypes[attrNum] == DT_STRING)
		{
			char* key = value->v.stringV;
			int key4 = schema->typeLength[attrNum];
			memmove(data, key, key4);
			data = data+key4;
		}	
		return RC_OK;
	}
}

RC getAttr (Record *record, Schema *schema, int attrNum, Value **value)
{
	int balance1 = 0;
	int *balance = &balance1;
	 *balance = 1;
	 if (schema == NULL)
	 {
	 	return -1;
	 }
	 else
	 {
	 	for (int i = 0; i < attrNum; ++i)
	{
		if (schema->dataTypes[i] == DT_INT)
		{
			*balance = *balance+a;
		}
		else if (schema->dataTypes[i] == DT_FLOAT)
		{
			*balance = *balance+b;
		}
		else if (schema->dataTypes[i] == DT_BOOL)
		{
			*balance = *balance+c;
		}
		else if (schema->dataTypes[i] == DT_STRING)
		{
			*balance = *balance+schema->typeLength[i];
		}
		else
		{
			printf("--------------------------Unknown datatype------------------------------\n");
			return -1;
		}
	}

	Value *attri = (Value *) malloc(sizeof(Value));
	char *data = record->data;
	data = data + balance1;

	if(attrNum != 1)
	{
		//do nothing
	}
	else
	{
		schema->dataTypes[attrNum] = 1;
	}
	int key=0;bool key2;float key3;
	if (schema->dataTypes[attrNum]== DT_STRING)
	{
		int key4 = schema->typeLength[attrNum];

		if (schema->typeLength[attrNum] !=0)
		{
			attri->v.stringV = (char*)malloc (key4+1);
		}
		else
		{
			return -1;
		}
		memmove(attri->v.stringV,data,key4);
		attri->dt = DT_STRING;
		*value = attri;
	}
	else if (schema->dataTypes[attrNum] == DT_INT)
	{
		memmove(&key,data,a);
		attri->dt = DT_INT;
		attri->v.intV = key;
		*value = attri;
	}
	else if (schema->dataTypes[attrNum] == DT_BOOL)
	{
		memmove(&key2,data,c);
		attri->dt = DT_BOOL;
		attri->v.boolV = key2;
		*value = attri;
	}
	else if (schema->dataTypes[attrNum] == DT_FLOAT)
	{
		memmove(&key3,data,b);
		attri->dt = DT_FLOAT;
		attri->v.boolV = key3;
		*value = attri;
	}
	*value = attri;
	return RC_OK;
	 }
	
}
