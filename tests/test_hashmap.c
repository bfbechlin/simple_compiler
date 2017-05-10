#include "hashmap.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_ITEMS 100000 /* max of 5 digits for non-duplicated keys */

int main(void)
{
	struct hashmap map;
	int value, i, r;
	char keys[NUM_ITEMS][10];
	int values[NUM_ITEMS];

	printf("initializing keys and values... ");
	for (i = 0; i < NUM_ITEMS; i++) {
		sprintf(keys[i], "%05d", i);
		values[i] = i;
	}
	printf("done.\n");

	assert(hm_initialize(2, 0.8, sizeof(int), &map) == 0);

	for (i = 0; i < NUM_ITEMS; i++) {
		assert(hm_put(&map, keys[i], &values[i]) == 0);
		assert(hm_get(&map, keys[i], &value) == 0);
		assert(value == values[i]);

		/* try to insert some random item already inserted */
		r = rand()%(i + 1);
		assert(hm_put(&map, keys[r], &values[r]) == -1);

		/* try to get some random item not inserted yet */
		r = (i + 1) + rand()%(NUM_ITEMS - i);
		assert(hm_get(&map, keys[r], &value) == -1);

		if ((i + 1)%(NUM_ITEMS/100) == 0)
			printf("\r%3d%% inserted... ", (100*(i + 1))/NUM_ITEMS);
		//hm_fprint(stdout, &map, 1);
	}
	printf("done.\n");

	for (i = 0; i < NUM_ITEMS; i++) {
		assert(hm_get(&map, keys[i], &value) == 0);
		assert(value == values[i]);
	}

	printf("final hashmap size: %u\n", map.size);


	printf("terminating hashmap... ");
	hm_terminate(&map);
	printf("done.\n");

	return 0;
}
