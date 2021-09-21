#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

#define READ_END 0
#define WRITE_END 1

int
main(int argc, char *argv[]) {
	if (argc < 2) {
		perror("cantidad de parametros incorrecta");
		_exit(-1);
	}
	pid_t cpid;
    int pipeRight[2] = {-1,-1};
    int pipeLeft[2] = {-1,-1};

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
		while(true) {
			close(pipeLeft[WRITE_END]);
			unsigned int prime = 0;
			int val = read(pipeLeft[READ_END],&prime,sizeof(unsigned int));
			if (val < 0) {
				perror("error en read");
				_exit(-1);
			} else if (val == 0) {
				close(pipeLeft[READ_END]);
				_exit(0);
			}

			fprintf(stdout,"%d\n",prime);
			fflush(stdout);

			if(pipeRight[READ_END] > 0 && pipeRight[WRITE_END] > 0) {
				close(pipeRight[READ_END]);
				close(pipeRight[WRITE_END]);
			}

			if (pipe(pipeRight) == -1) {
				perror("error en pipe");
				_exit(-1);
			}

			cpid = fork();
			if (cpid < 0) {
				perror("error en fork\n");
				_exit(-1);
			}

			if (cpid == 0) {
				close(pipeLeft[READ_END]);
				pipeLeft[READ_END] = pipeRight[READ_END];
				pipeLeft[WRITE_END] = pipeRight[WRITE_END];
				pipeRight[READ_END] = -1;
				pipeRight[WRITE_END] = -1;
			} else {
				unsigned int n = 0;
				while (read(pipeLeft[READ_END],&n,sizeof(unsigned int)) > 0) {
					if (n % prime != 0) {
						if(write(pipeRight[WRITE_END],&n,sizeof(unsigned int)) < 0) {
							perror("error en write");
							_exit(-1);
						}
					}
				}

				close(pipeLeft[READ_END]);
				close(pipeRight[WRITE_END]);
				wait(NULL);
				_exit(0);
			}
		}
	} else {
		close(pipeLeft[READ_END]);
		unsigned int n = atoi(argv[1]);
		for (unsigned int i = 2; i <= n; i++) {
			if(write(pipeLeft[WRITE_END],&i,sizeof(unsigned int)) < 0) {
				perror("error en write");
				_exit(-1);
			}
		}

		close(pipeLeft[WRITE_END]);
		wait(NULL);
		_exit(0);
	}	
}

