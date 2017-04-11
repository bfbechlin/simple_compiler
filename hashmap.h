#ifndef HASHMAP_H
#define HASHMAP_H 1

#include <stdlib.h>

struct hm_bucket {
	char *key;
	void *value;
};

struct hashmap {
	unsigned int size;
	unsigned int used;
	float load_factor;

	size_t value_size; /* sizeof(<type to be stored>) */
	struct hm_bucket **buckets;
};

int hm_init(unsigned int size, float load_factor, size_t value_size, struct hashmap *hm);

int hm_put(struct hashmap *hm, const char *key, const void *value);
int hm_get(struct hashmap *hm, const char *key, void *value);

#endif /* ifndef HASHMAP_H */
