#include <string.h>

#include "hashmap.h"

/* Algorithm from:
 * http://stackoverflow.com/questions/7666509/hash-function-for-string */
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

static void hm_terminate_item_list(struct hm_item *first_item) {
	if (first_item == NULL) {
		return;
	}

	hm_terminate_item_list(first_item->next);
	free(first_item->key);
	free(first_item->value);
	free(first_item);
}

void hm_terminate(struct hashmap *hm) {
	int i;

	for (i = 0; i < hm->size; i++) {
		hm_terminate_item_list(hm->buckets[i]);
	}

	free(hm->buckets);
}

/* Double the size of the hashmap. */
static int hm_grow(struct hashmap *hm) {
	/* remember old size and buckets */
	int i;
	unsigned int old_size = hm->size;
	struct hm_item **old_buckets = hm->buckets;

	/* alloc new buckets */
	hm->size = 2*hm->size;
	hm->used = 0;
	hm->buckets = calloc(hm->size, sizeof(struct hm_item *));

	if (!hm->buckets) {
		return -1;
	}

	/* insert old values */
	struct hm_item *item;
	for (i = 0; i < old_size; i++) {
		item = old_buckets[i];
		while (item != NULL) {
			hm_put(hm, item->key, item->value);
			item = item->next;
		}

		hm_terminate_item_list(old_buckets[i]);
	}

	free(old_buckets);

	return 0;
}

int hm_put(struct hashmap *hm, const char *key, const void *value) {
	int index = hm_hash_function(key)%hm->size;

	struct hm_item **slot;
	slot = hm->buckets + index;

	while (*slot != NULL && strcmp((*slot)->key, key) != 0) {
		slot = &((*slot)->next);
	}

	if (*slot != NULL) {
		/* key already present */
		return -1;
	}

	struct hm_item *new = malloc(sizeof(struct hm_item));
	if (new == NULL) {
		return -2;
	}

	new->key = malloc(strlen(key) + 1);
	new->value = malloc(hm->value_size);
	if (new->key == NULL || new->value == NULL) {
		free(new);
		return -2;
	}

	strcpy(new->key, key);
	memcpy(new->value, value, hm->value_size);
	new->next = NULL;
	*slot = new;

	hm->used++;

	if ((float)hm->used/hm->size > hm->load_factor) {
		hm_grow(hm);
	}

	return 0;
}

int hm_get(struct hashmap *hm, const char *key, void *value) {
	int index = hm_hash_function(key)%hm->size;

	struct hm_item **item;
	item = hm->buckets + index;

	while (*item != NULL && strcmp((*item)->key, key) != 0) {
		item = &((*item)->next);
	}

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
