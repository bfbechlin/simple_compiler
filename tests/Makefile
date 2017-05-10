ALL_TARGETS := test_hashmap

.PHONY = all
all: $(ALL_TARGETS)

test_hashmap: test_hashmap.c ../hashmap.c
	gcc -I .. -o test_hashmap $^
	./test_hashmap

.PHONY = clean
clean:
	rm -rf $(ALL_TARGETS)
