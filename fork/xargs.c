#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>

#ifndef NARGS
#define NARGs 4
#endif

#define NEWLINE '\n'
#define NULLCHAR '\0'
#define BUFFERSIZE 1024

int trimNewLine(char* str);

int
main(int argc, char *argv[]) {
	if (argc < 2) {
		perror("cantidad de argumentos incorrecta");
		_exit(EXIT_FAILURE);
	}
	
	pid_t cpid;
	char* command = argv[1];
	char* line = NULL;
	size_t len;
	ssize_t nread = 0;
	int argsRead = 1;
	char argArr[NARGs][BUFFERSIZE]; 
	char* args[NARGs + 2];
	args[0] = command;

	while(argsRead > 0) {
		argsRead = 0;
		for (size_t i = 1; i <= NARGs; i++) {		
			if ((nread = getline(&line,&len,stdin)) != -1) {
				trimNewLine(line);
				memset(argArr[i],0,BUFFERSIZE);
				strncpy(argArr[i],line,nread);
				args[i] = *(argArr + i);
				argsRead ++;
			}
		}

		args[argsRead+1] = NULL;
	
		 if(argsRead > 0) {			
			cpid = fork();
			if (cpid < 0) {
				perror("error en fork");
				_exit(EXIT_FAILURE);
			}

			if (cpid == 0) {
				execvp(command,args);
				perror("error en exec");
				_exit(EXIT_FAILURE);
			} else {
				if (wait(NULL) < 0) {
					perror("error en wait");
					_exit(EXIT_FAILURE);
				}
				printf("%c",NEWLINE);
			}
		} 
	}

	free(line);
	_exit(EXIT_SUCCESS);
}

int trimNewLine(char* str) {
	char* ret;
	ret = strchr(str,NEWLINE);
	if (ret) {
		*ret = NULLCHAR;
		return 0;
	}
	return -1;
}
