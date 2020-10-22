CC = gcc
CFLAGS  = -g -Wall
 
default: test1 test2

test1: test_assign4_1.o btree_mgr.o dberror.o expr.o record_mgr.o rm_serializer.o storage_mgr.o buffer_mgr.o buffer_mgr_stat.o
	$(CC) $(CFLAGS) -o test1 test_assign4_1.o btree_mgr.o dberror.o expr.o record_mgr.o rm_serializer.o storage_mgr.o buffer_mgr.o buffer_mgr_stat.o 

test2: test_expr.o btree_mgr.o dberror.o expr.o record_mgr.o rm_serializer.o storage_mgr.o buffer_mgr.o buffer_mgr_stat.o
	$(CC) $(CFLAGS) -o test2 test_expr.o btree_mgr.o dberror.o expr.o record_mgr.o rm_serializer.o storage_mgr.o buffer_mgr.o buffer_mgr_stat.o 

clean: 
	$(RM) test1 test2 *.o *~

run:
	./test1 && ./test2