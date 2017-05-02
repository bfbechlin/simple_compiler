#include <stdlib.h>
#include "hashmap.h"
#include "symbol_table.h"

struct hashmap hash;

void symtab_init(void){
	hm_initialize(8, 0.5, sizeof(int), &hash);
}

void symtab_insert(char* symbol, int code){
	hm_put(&hash, symbol, &code);
}

void symtab_print(void){
	printf("\nSYMBOLS TABLE:\n");
	hm_fprint(stdout, &hash, 0);
}

void symtab_destroy(void){
	hm_terminate(&hash);
}
