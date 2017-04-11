#include "hashmap.h"
#include <assert.h>


int main(void)
{
	struct hashmap map;
	int value, i, num = 3;
	const char *keys[] = {"key1", "key2", "key3"};
	int values[] = {1, 2, 3};

	assert(hm_init(10, 0.6, sizeof(int), &map) == 0);

	for (i = 0; i < num; i++) {
		assert(hm_put(&map, keys[i], &values[i]) == 0);
		assert(hm_get(&map, keys[i], &value) == 0);
		assert(value == values[i]);
	}

	return 0;
}
