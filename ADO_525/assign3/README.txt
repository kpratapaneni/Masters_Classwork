_____________ Executing the codes ________________________________________________________________________________________


* go to root directory and type the following:
	> make clean;
	> make;
	> make run;


_____________ Function description ________________________________________________________________________________________


initRecordManager()
- initializes the record manager and storage manager

shutdownRecordManager()
- closes record manager and frees allocated resources currently in use

findSlot()
- returns the position of the next empty slot in the table

checkresult()
- returns RC_OK if the result is non-null

createTable()
- creates a table with a requierd name(argument passed to fn), initializes the buffer pool, pages, and all the required table values

openTable()
- loads a requested table with the required name(argument passed to fn)

closeTable()
- closes the table currently being pointed to and frees allocated memory by invoking shutdownBufferPool() after writing any modifications made to the tables

deleteTable()
- deletes a table with a required name (argument) and frees up occupied memory

getNumTuples()
- returns the number of records in the currently referenced table

insertRecord()
- adds a record to the table, and marks the corresponding page as dirty, so that it can be flushed to disk by buffer manager

deleteRecord()
- removes a record from the table, and marks the corresponding page as dirty, so that the changes are flushed to disk by buffer manager

updateRecord()
- updates a referenced record in the table , and marks the corresponding page as dirty and pins it to the buffer pool, so that the changes are flushed to disk by buffer manager

getRecord()
- retrieves a particular record with matching record ID (argument passed to fn)

startScan()
- scans the scan handle structure. if the condition is null, error code is returned

next()
- returns the next record that satisfies a condition/expression. If no records satisfy, then an error is thrown, and the scan is closed. IF satisfied, we pin pages that contain the tuples amd evaluate the expression on those pages. 

closeScan() 
- closes the  current scan, and unpins any pinned pages, and frees up data.

resetScanManager()
- resets parameters of the TableManager object

getRecordSize()
- finds and returns size of a row in a schema based on sizes of each of its columns/attributes.

freeSchema()
- removes the schema passed as an argument from the memory and frees up used space

createSchema()
- creates a new schema based on the specified memory parameters and attribute data type specifications

createRecord()
- creates and returns a new record according to a schema and allocates memory for it.

freeRecord()
- frees up memory allocated to a particular record

getAttr()
- extracts an attribute from the record using the schema(passed as argument to the fn)

setAttr()
- sets a value for a particular attribute in a record in the schema (passed as argument)