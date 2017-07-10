#ifndef ASSEMBLY_H
#define ASSEMBLY_H 0

#include <stdio.h>

#include "tac.h"
#include "assembly.h"

/* Print a TAC instruction list */
void fprint_assembly(FILE *stream, struct tac *list);

#endif /* ASSEMBLY_H */
