#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

#define READ_END 0
#define WRITE_END 1

int
main(int argc, char *argv[]) {
    // Your code here
	if (argc < 2)
		perror("cantidad de parametros incorrecta");
	pid_t cpid;
    int pipeRight[2];
    int pipeLeft[2];

	if (pipe(pipeLeft) == -1) {
        perror("error en pipe");
        _exit(-1);
    }

	cpid = fork();
	if (cpid == -1) {
		perror("error en fork");
		_exit(-1);
	}

	if (cpid == 0) {
		//child process
		close(pipeLeft[WRITE_END]);
		cpid = fork();
		fprintf(stdout,"child process running\n");
		unsigned int prime = 0;
		read(pipeLeft[READ_END],&prime,sizeof(unsigned int));
		fprintf(stdout,"%d\n",prime);
		fflush(stdout);

		unsigned int n = 0;
		while (read(pipeLeft[READ_END],&n,sizeof(unsigned int)) > 0) {
			if (n % prime != 0) {
				write(pipeRight[WRITE_END],&n,sizeof(unsigned int));
			}
		}

		close(pipeLeft[READ_END]);
		close(pipeRight[WRITE_END]);
		wait(NULL);
		_exit(0);
	} else {
		close(pipeLeft[READ_END]);
		//parent process
		fprintf(stdout,"parent process running\n");
		fflush(stdout);
		unsigned int n = atoi(argv[1]);
		bool error  = false;
		for (unsigned int i = 2; (i <= n && !error); i++) {
			if(write(pipeLeft[WRITE_END],&i,sizeof(unsigned int)) < 0) {
				perror("error en write");
				error = true;
			}
		}
		close(pipeLeft[WRITE_END]);
		
		wait(NULL);
		if (error) {
			_exit(-1);
		}
		_exit(0);
	}	
}

