#include <string.h>
#include <stdio.h>

#include "hashmap.h"

/*
 * Algorithm from:
 * http://stackoverflow.com/questions/7666509/hash-function-for-string
 */
static unsigned int hm_hash_function(const char *key) {
	int i = 0;
	char c = key[i];
	unsigned int hash = 5381;

	while(c != '\0') {
		hash = ((hash << 5) + hash) + c;
		c = key[i++];
	}

	return hash;
}

int hm_init(unsigned int size, float load_factor, size_t value_size, struct hashmap *hm) {
	struct hm_bucket **buckets;
	buckets = calloc(size, sizeof(struct hm_bucket *));

	if (!buckets) {
		return -1;
	}

	hm->size = size;
	hm->used = 0;
	hm->load_factor = load_factor;

	hm->value_size = value_size;
	hm->buckets = buckets;

	return 0;
}

int hm_put(struct hashmap *hm, const char *key, const void *value) {
	int index = hm_hash_function(key)%hm->size;

	if (hm->buckets[index] != NULL) {
		return -1;
	}

	struct hm_bucket *new = malloc(sizeof(struct hm_bucket));
	if (!new) {
		return -2;
	}


	new->key = malloc(strlen(key) + 1);
	strcpy(new->key, key);
	new->value = malloc(hm->value_size);
	memcpy(new->value, value, hm->value_size);
	hm->buckets[index] = new;

	return 0;
}

int hm_get(struct hashmap *hm, const char *key, void *value) {
	int index;

	if (!value) {
		return -1;
	}

	index = hm_hash_function(key)%hm->size;

	if (!hm->buckets[index] && !strcmp(hm->buckets[index]->key, key)) {
		return -2;
	}

	memcpy(value, hm->buckets[index]->value, hm->value_size);

	return 0;
}
