#include "hashmap.h"
#include <assert.h>
#include <stdio.h>

#define NUM_ITEMS 10 /* maximum of 3 digits! */

int main(void)
{
	struct hashmap map;
	int value, i, num = 8;
	char keys[NUM_ITEMS][5];
	int values[NUM_ITEMS];

	for (i = 0; i < NUM_ITEMS; i++) {
		sprintf(keys[i], "k%03d", i);
		values[i] = i;
	}

	assert(hm_initialize(1, 0.8, sizeof(int), &map) == 0);

	for (i = 0; i < NUM_ITEMS; i++) {
		printf("inserting `%s`...\n", keys[i]);
		assert(hm_put(&map, keys[i], &values[i]) == 0);
		assert(hm_get(&map, keys[i], &value) == 0);
		assert(value == values[i]);
		hm_fprint(stdout, &map, 1);
		printf("\n");
	}

	for (i = 0; i < num; i++) {
		assert(hm_get(&map, keys[i], &value) == 0);
		assert(value == values[i]);
	}

	printf("%u\n", map.size);


	hm_terminate(&map);

	return 0;
}
