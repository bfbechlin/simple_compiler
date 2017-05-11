#include <stdlib.h>
#include "astree.h"
#include "hashmap.h"

int main(int argc, char *argv[])
{
	struct hashmap map;
	hm_initialize(10, 0.8, 1, &map);

	/* put some garbage in the hashmap */
	int n1, n2;
	hm_put(&map, "1", &n1);
	hm_put(&map, "2", &n2);
	struct hm_item *s1 = hm_getref(&map, "1");
	struct hm_item *s2 = hm_getref(&map, "2");

	/* build 1 + 2 */
	struct astree *t1 = ast_create(AST_SYM, s1, NULL, NULL, NULL, NULL);
	struct astree *t2 = ast_create(AST_SYM, s2, NULL, NULL, NULL, NULL);
	struct astree *sum = ast_create(AST_ADD, NULL, t1, t2, NULL, NULL);
	ast_fprint(stderr, 0, sum);

	hm_terminate(&map);
	free(t1);
	free(t2);
	free(sum);
	return 0;
}
