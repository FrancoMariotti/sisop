#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#ifndef NARGS
#define NARGs 4
#endif

#define NEWLINE '\n'
#define NULLCHAR '\0'

size_t trimNewLine(char* str);

int
main(int argc, char *argv[]) {
	if (argc < 2) {
		perror("cantidad de argumentos incorrecta");
		_exit(EXIT_FAILURE);
	}
	char* command = argv[1];
	printf("el comando leido es:%s\n",command);
	char* linePtr = NULL;
	size_t len;
	ssize_t nread = 0;

	while(nread >= 0) {
		for (size_t i = 0; i < NARGs; i++) {		
			if ((nread = getline(&linePtr,&len,stdin)) != -1) {
				len = trimNewLine(linePtr);
				printf("linea leida: %s\n",linePtr);
			}
		}

		//aca quedaria hacer un fork()
		//que el hijo haga un execvp
		//y el padre haga un wait.
		//chequear errores y listo.
		printf("no termine\n");
	}


	free(linePtr);
	_exit(EXIT_SUCCESS);
}

size_t trimNewLine(char* str) {
	char* ret;
	ret = strchr(str,NEWLINE);
	if (ret) {
		*ret = NULLCHAR;
		return strlen(str) - 1;
	}

	return strlen(str);
}