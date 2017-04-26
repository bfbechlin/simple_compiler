#include <stdlib.h>
#include "hashmap.h"
#include "symbols_table.h"

struct hashmap hash;

void symtab_init(void){
	hm_initialize(8, 0.5, sizeof(int), &hash);
}

void symtab_insert(char* symbol, int code){
	hm_put(&hash, symbol, &code);
}

void symtab_print(void){
	hm_fprint(stdin, &hash, 0);
}

void symtab_destroy(void){
	hm_terminate(&hash);
}
