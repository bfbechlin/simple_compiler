#ifndef HASHMAP_H
#define HASHMAP_H 1

#include <stdio.h>

/* linked list item in hashmap */
struct hm_item {
	char *key;
	void *value;
	struct hm_item *next;
};

struct hashmap {
	unsigned int size;
	unsigned int used;
	float load_factor; /* float between 0 and 1 */

	size_t value_size; /* sizeof(<type to be stored>) */
	struct hm_item **buckets;
};

/* Allocate buckets and set struct values. */
int hm_initialize(unsigned int size, float load_factor, size_t value_size, struct hashmap *hm);

/* Deallocate buckets. */
void hm_terminate(struct hashmap *hm);

/* Insert (key, value) pair. */
int hm_put(struct hashmap *hm, const char *key, const void *value);

/* Retrieve a reference to an item */
struct hm_item *hm_getref(struct hashmap *hm, const char *key);

/* Retrive (key, value) pair. */
int hm_get(struct hashmap *hm, const char *key, void *value);

/* Remove (key, value) pair. */
int hm_remove(struct hashmap *hm, const char *key);

/* Write representation of hashmap keys to stream. */
void hm_fprint(FILE *stream, struct hashmap *hm, char print_empty);

#endif /* ifndef HASHMAP_H */
