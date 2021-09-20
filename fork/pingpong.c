/*Se pide escribir un programa en C que use fork(2) y pipe(2) para enviar y recibir(ping-pong) un determinado valor entero, entre dos procesos.
 * El valor se debe crear con random(3) una vez ambos procesos existan.
 *
 * */

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "time.h"

#define READ_END 0
#define WRITE_END 1

void closePipe(int* pipefd); 

int main(/*int argc,char** argv*/) {
	int pipefd1[2];
	int pipefd2[2];
	long int buf = -1;		
	pid_t cpid;


	if (pipe(pipefd1) < 0) {
		perror("error en la creacion del pipe");
		exit(EXIT_FAILURE);
	}
	
	if (pipe(pipefd2) < 0) {
		closePipe(pipefd1);
		perror("error en la creacion del pipe");
		exit(EXIT_FAILURE);
	}

	printf("Hola, soy PID %d:\n",getpid());	
	printf("\t-primer pipe me devuelve:[%d,%d]\n",pipefd1[READ_END],pipefd1[WRITE_END]);
	printf("\t-segundo pipe me devuelve:[%d,%d]\n",pipefd2[READ_END],pipefd2[WRITE_END]);

	cpid = fork();
	if (cpid < 0) {
		closePipe(pipefd1);
		closePipe(pipefd2);
		perror("error en fork\n");
		exit(EXIT_FAILURE);
	}
	
	printf("Donde fork me devuelve %d:\n"
		"\t-getpid me devuelve:%d\n"
		"\t-getppid me devuelve:%d\n",
		cpid,getpid(),getppid());

	if (cpid == 0) { /*proceso hijo*/
		close(pipefd1[WRITE_END]); //cierro extremo de escritura del pipe 1
		close(pipefd2[READ_END]); //cierro el extremo de lectura del pipe 2
		
		int valueRead = read(pipefd1[READ_END],&buf,sizeof(buf));
		if(valueRead < 0) {
			perror("proceso hijo: error en read");
			close(pipefd1[READ_END]);
			close(pipefd2[WRITE_END]);
			exit(EXIT_FAILURE);
		} 

		printf("\t-recibo valor %ld via fd=%d\n"
			"\t-reenvio valor en fd=%d y termino\n",
			buf,pipefd1[READ_END],pipefd2[WRITE_END]);	
		
		int valueWrite = write(pipefd2[WRITE_END],&buf,sizeof(buf));
		close(pipefd1[READ_END]); //cierro extremo de lectura del pipe 1
		close(pipefd2[WRITE_END]); //cierro el extremo de escritura del pipe 2
		
		if (valueWrite < 0) {
			perror("proceso hijo: error en write");
			exit(EXIT_FAILURE);
		}
	} else {
		close(pipefd1[READ_END]); //cierro el extremo de lectura del pipe 1
		close(pipefd2[WRITE_END]); //cierro extremo de escritura del pipe 2

		srandom(time(NULL));
		long int rand = random();
		printf("\t-random me devulve:%ld\n"
				"\t-envio valor %ld via fd=%d\n",
				rand,rand,pipefd1[WRITE_END]);

		int valueWrite =  write(pipefd1[WRITE_END],&rand,sizeof(rand));
		if (valueWrite < 0) {
			perror("proceso padre: error en la escritura");
			close(pipefd1[WRITE_END]);
			close(pipefd2[READ_END]);
			exit(EXIT_FAILURE);
		} 

		int valueRead = read(pipefd2[READ_END],&buf,sizeof(buf));
		if(valueRead < 0) {
			perror("proceso padre: error en read");
			close(pipefd1[WRITE_END]);
			close(pipefd2[READ_END]);
			exit(EXIT_FAILURE);
		} 

		printf("Hola, de nuevo PID %d:\n"
				"\t- recibi valor %ld via fd=%d\n",
				getpid(),buf,pipefd2[READ_END]);
		
		close(pipefd1[WRITE_END]); //cierro extremo de escritura del pipe 1
		close(pipefd2[READ_END]); //cierro el extremo de lectura del pipe 2
	}


	exit(EXIT_SUCCESS);
}

void closePipe(int* pipefd) {
	close(pipefd[0]);
	close(pipefd[1]);
}
