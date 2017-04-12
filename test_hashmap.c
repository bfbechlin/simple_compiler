#include "hashmap.h"
#include <assert.h>
#include <stdio.h>

int main(void)
{
	struct hashmap map;
	int value, i, num = 5;
	const char *keys[] = {"key1", "key2", "key3", "key4", "key5"};
	int values[] = {1, 2, 3, 4, 5};

	assert(hm_initialize(2, 0.6, sizeof(int), &map) == 0);

	for (i = 0; i < num; i++) {
		printf("%d\n", i);
		assert(hm_put(&map, keys[i], &values[i]) == 0);
		assert(hm_get(&map, keys[i], &value) == 0);
		assert(value == values[i]);
	}

	for (i = 0; i < num; i++) {
		assert(hm_get(&map, keys[i], &value) == 0);
		assert(value == values[i]);
	}

	assert(map.size >= num);
	printf("%u\n", map.size);

	hm_terminate(&map);

	return 0;
}
