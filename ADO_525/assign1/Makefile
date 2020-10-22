CC=gcc
CFLAGS=-I.

tcase: test_assign1_1.c storage_mgr.c dberror.c
	gcc -o tcase test_assign1_1.c storage_mgr.c dberror.c -I.

clean:
	rm -rf *.o
	rm -rf tcase
