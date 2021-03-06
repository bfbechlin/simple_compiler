#include <stdlib.h>
#include <stdio.h>
#include "hashmap.h"
#include "symbol_table.h"

static struct hashmap hash;

void symtab_init(void){
	hm_initialize(8, 0.5, sizeof(struct symtab_item), &hash);
}

struct hm_item *symtab_insert(const char* symbol, int code){
	struct symtab_item item;
	item.code = code;
	item.data_type = 0;
	item.id_type = 0;
	item.decl = NULL;

	hm_put(&hash, symbol, &item);
	return hm_getref(&hash, symbol);
}

int symtab_get(const char* symbol, struct symtab_item* dummy){
	return hm_get(&hash, symbol, (void*) dummy);
}

void symtab_print(void){
	hm_fprint(stdout, &hash, 0);
}

static const char *data_type_to_string[] = {
	"", "double", "", "float", "", "long", "", "",
	"", "", "", "", "", "", "", "short",
	"", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "byte",
	"boolean"
};

static const char *id_type_to_string[] = {
	"",
	"var",
	"vec",
	"fun"
};


void symtab_fprint_item(FILE *stream, struct symtab_item *item) {
	fprintf(stream, "data_t: %s, id_t: %s",
	        data_type_to_string[item->data_type],
	        id_type_to_string[item->id_type]);
}

void symtab_destroy(void){
	hm_terminate(&hash);
}
