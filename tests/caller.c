#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
	This program is used to call a c executable n times for a set of arguments.
	IMPORTANT: the called program must execute with only one argument.

	USAGE:
	./caller <called_program> args
	Example:
	./caller ./test 1 2 3 4 5
	It will execute 5 times test program showing the exit status of the program.

	Pratical use:
	ls -d  $PWD/*.* | xargs ./caller ./test
	It will list all files in simple_path and redirect to caller execute with the
	name of this file de test program.

	*Semantic Tests*
		ls -d  $PWD/semantic_tests/*.txt | xargs ./caller ../etapa4
*/



static void file_name(char* const path, char *buffer){
	int i, len = strlen(path);
	for(i = len; i >= 0; i--)
		if(path[i] == '\\' || path[i] == '/')
			break;
	if(path[i] == '\\' || path[i] == '/')
		i++;
	strncpy(buffer, &path[i], len);
}

int main(int argc, char* argv[]){
	int i;
	int* result;
	char command[2024];

	if(argc < 2)
		fprintf(stderr, "Error. Program to be called not defined!!!\n");
	char *program = argv[1];

	result = (int *)calloc(argc, sizeof(int));

	for(i = 2; i < argc ; i++){
		snprintf(command, sizeof(command), "%s %s", program, argv[i]);
		result[i] = system(command);
	}


	for(i = 2; i < argc ; i++){
		file_name(argv[i], command);
		printf("%-73sexit(%i)\n", command, result[i]/256);
	}

	free(result);
	return 0;
}
