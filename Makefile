default:
	gcc -std=c99 test_hashtable.c -o test_hashtable
	gcc -std=c99 test_skiplist.c -o test_skiplist

clean:
	rm -f test_hashtable test_skiplist