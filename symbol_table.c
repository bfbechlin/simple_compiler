#include <stdlib.h>
#include <stdio.h>
#include "hashmap.h"
#include "symbol_table.h"

struct hashmap hash;

void symtab_init(void){
	hm_initialize(8, 0.5, sizeof(int), &hash);
}

struct hm_item *symtab_insert(char* symbol, int code){
	hm_put(&hash, symbol, &code);
	return hm_getref(&hash, symbol);
}

void symtab_print(void){
	printf("\nSYMBOLS TABLE:\n");
	hm_fprint(stdout, &hash, 0);
}

void symtab_destroy(void){
	hm_terminate(&hash);
}
