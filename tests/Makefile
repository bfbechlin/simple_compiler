COMP_BIN := ../etapa4
ALL_TARGETS := caller test_hashmap

.PHONY = all
all: $(ALL_TARGETS)

test_hashmap: test_hashmap.c ../hashmap.c
	gcc -I .. -o test_hashmap $^

test_astree: test_astree.c ../hashmap.c ../astree.c
	gcc -Wall -g -I .. -o test_astree $^

caller: caller.c
	gcc -o caller caller.c

.PHONY = clean semantic_test
clean:
	rm -rf $(ALL_TARGETS)

semantic_test: caller $(COMP_BIN)
		ls -d  $(PWD)/semantic/*.txt | xargs ./caller $(COMP_BIN)
