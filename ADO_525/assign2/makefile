CC = gcc
CFLAGS  = -g -Wall 
 
default: tcase1 tcase2

tcase1: test_assign2_1.o storage_mgr.o dberror.o buffer_mgr.o buffer_mgr_stat.o
	$(CC) $(CFLAGS) -o tcase1 test_assign2_1.o storage_mgr.o dberror.o buffer_mgr.o buffer_mgr_stat.o -lm

tcase2: test_assign2_2.o storage_mgr.o dberror.o buffer_mgr.o buffer_mgr_stat.o
	$(CC) $(CFLAGS) -o tcase2 test_assign2_2.o storage_mgr.o dberror.o buffer_mgr.o buffer_mgr_stat.o -lm

clean: 
	$(RM) tcase1 tcase2 *.o *~

run1:
	./tcase1

run2:
	./tcase2