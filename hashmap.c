#include <string.h>
#include <stdlib.h>
#include "hashmap.h"

/* Algorithm from:
 * http://stackoverflow.com/questions/7666509/hash-function-for-string */
static unsigned int hm_hash_function(const char *key) {
	const char *c = key;
	unsigned int hash = 5381;

	while(*c != '\0') {
		hash = ((hash << 5) + hash) + *c;
		c++;
	}

	return hash;
}

void hm_fprint(FILE *stream, struct hashmap *hm, char print_empty) {
	int i;
	struct hm_item *item;
	for (i = 0; i < hm->size; i++) {
		item = hm->buckets[i];

		if (!print_empty && item == NULL) {
			continue;
		}

		fprintf(stream, "%3d: ", i);
		while (item != NULL) {
			fprintf(stream, "`%s` -> ", item->key);
			item = item->next;
		}
		fprintf(stream, "NULL\n");
	}
}

int hm_initialize(unsigned int size, float load_factor, size_t value_size, struct hashmap *hm) {
	struct hm_item **buckets;
	buckets = calloc(size, sizeof(struct hm_item *));

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

static void hm_terminate_bucket(struct hm_item *first_item) {
	if (first_item == NULL) {
		return;
	}

	hm_terminate_bucket(first_item->next);
	free(first_item->key);
	free(first_item->value);
	free(first_item);
}

void hm_terminate(struct hashmap *hm) {
	int i;

	for (i = 0; i < hm->size; i++) {
		hm_terminate_bucket(hm->buckets[i]);
	}

	free(hm->buckets);
}

/* Return a pointer to the slot where a key is or to where it should be if
 * inserted next.
 *
 * Example:
 *
 * If hash("bar")%size == 0, and hm->buckets is:
 *
 *     0: NULL
 *     1: `foo` -> `baz` -> NULL
 *     2: NULL
 *     3: NULL
 *
 * Giving hm_find the key "foo", it'll return a pointer to the second
 * position of hm->buckets, and dereferencing that pointer
 * would give you the address of the item with key "foo".
 *
 * Giving hm_find the key "bar", it'll return a pointer to the first
 * position of hm->buckets, and dereferencing that pointer would give you
 * NULL.
 *
 * Giving hm_find the key "baz", it'll return a pointer to the `next`
 * member of the item with key "foo", and dereferencing the pointer would give
 * you the address of the item with key "baz".
 */
static struct hm_item **hm_find(struct hashmap *hm, const char *key) {
	int index = hm_hash_function(key)%hm->size;

	struct hm_item **slot;
	slot = hm->buckets + index;

	while (*slot != NULL && strcmp((*slot)->key, key) != 0) {
		slot = &((*slot)->next);
	}

	return slot;
}

/* Double the size of the hashmap. */
static int hm_grow(struct hashmap *hm) {
	/* remember old size and buckets */
	int i;
	unsigned int old_size = hm->size;
	struct hm_item **old_buckets = hm->buckets;

	/* alloc new buckets */
	hm->size = 2*hm->size;
	hm->buckets = calloc(hm->size, sizeof(struct hm_item *));

	if (!hm->buckets) {
		return -1;
	}

	/* insert old values */
	struct hm_item *curr_item, *next_item;
	struct hm_item **new_slot;
	for (i = 0; i < old_size; i++) {
		curr_item = old_buckets[i];
		while (curr_item != NULL) {
			new_slot = hm_find(hm, curr_item->key);
			*new_slot = curr_item;

			next_item = curr_item->next;
			curr_item->next = NULL;
			curr_item = next_item;
		}
	}

	free(old_buckets);

	return 0;
}

int hm_put(struct hashmap *hm, const char *key, const void *value) {
	struct hm_item **slot = hm_find(hm, key);

	if (*slot != NULL) {
		/* key already present */
		return -1;
	}

	/* Before anyone complains about the use of goto:
	 * http://stackoverflow.com/questions/245742/examples-of-good-gotos-in-c-or-c */
	struct hm_item *new = malloc(sizeof(struct hm_item));
	if (new == NULL) goto alloc_fail1;
	new->key = malloc(strlen(key) + 1);
	if (new->key == NULL) goto alloc_fail2;
	new->value = malloc(hm->value_size);
	if (new->value == NULL)	goto alloc_fail3;

	strcpy(new->key, key);
	memcpy(new->value, value, hm->value_size);
	new->next = NULL;
	*slot = new;

	hm->used++;

	if ((float)hm->used/hm->size > hm->load_factor) {
		hm_grow(hm);
	}

	return 0;

alloc_fail3:
	free(new->key);
alloc_fail2:
	free(new);
alloc_fail1:
	return -2;
}

struct hm_item *hm_getref(struct hashmap *hm, const char *key) {
	struct hm_item **item = hm_find(hm, key);

	return *item;
}

int hm_get(struct hashmap *hm, const char *key, void *value) {
	struct hm_item **item = hm_find(hm, key);

	if (*item == NULL) {
		/* item does not exist */
		return -1;
	}

	memcpy(value, (*item)->value, hm->value_size);

	return 0;
}

int hm_remove(struct hashmap *hm, const char *key) {
	return 0;
}
