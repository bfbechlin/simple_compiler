ALL_TARGETS := test_hashmap test_astree

.PHONY = all
all: $(ALL_TARGETS)

test_hashmap: test_hashmap.c ../hashmap.c
	gcc -I .. -o test_hashmap $^

test_astree: test_astree.c ../hashmap.c ../astree.c
	gcc -Wall -g -I .. -o test_astree $^

.PHONY = clean
clean:
	rm -rf $(ALL_TARGETS)
