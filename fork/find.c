#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

#define MAX_PATH 251
#define SEPARADOR "/"
#define ROOT_DIRECTORY "."
#define PREVIOUS_DIRECTORY ".."

typedef char* (*cmp_t)(const char*,const char* needle);
void printOcurrencies(DIR* folder,char* path,char* pattern,cmp_t cmp);

int
main(int argc, char *argv[]) {
	int opt;
	cmp_t ptr_cmp  = &strstr;
	while ((opt = getopt(argc, argv, "i")) != -1) {
		switch (opt) {
			case 'i':
				ptr_cmp = &strcasestr;
				break;
			default:
				fprintf(stderr, "Usage: %s [-i] pattern\n",
						argv[0]);
				_exit(EXIT_FAILURE);
		}
	}
        
	if (optind >= argc) {
		perror("Expected argument after options");
		_exit(EXIT_FAILURE);
	}

	char* cadena = argv[optind];
	char path[MAX_PATH];
	memset(path,0,MAX_PATH);
	DIR* folder = opendir(ROOT_DIRECTORY);
	if (!folder) {
		perror("error en opendir");
		_exit(EXIT_FAILURE);
	}

	printOcurrencies(folder,path,cadena,ptr_cmp);
	closedir(folder);
	_exit(EXIT_SUCCESS);
}


void printOcurrencies(DIR* folder,char* path,char* pattern,cmp_t cmp) {
	struct dirent *entry;
	while ((entry = readdir(folder))) {
		if (entry->d_type == DT_REG) {
			char* ret = cmp(entry->d_name,pattern);
			if (ret) {
				printf("%s%s\n",path,entry->d_name);
			}
		} else if (entry->d_type == DT_DIR) {
			if(strncmp(entry->d_name,ROOT_DIRECTORY,1) == 0 || strncmp(entry->d_name,PREVIOUS_DIRECTORY,2) == 0)
				continue;
			int fdRoot = dirfd(folder);
			if (fdRoot == -1){
				perror("error en dirfd");
				continue;
			}

			int fdSubdir = openat(fdRoot,entry->d_name,O_DIRECTORY);
			if (fdSubdir == -1){
				perror("error en openat");
				continue;
			}

			DIR* subdir = fdopendir(fdSubdir);
			if (!subdir){
				close(fdSubdir);
				perror("error en fdopendir");
				continue;
			}

			strncat(path,entry->d_name,MAX_PATH-strlen(path)-1);
			strncat(path,SEPARADOR,MAX_PATH-strlen(path)-1);
			printOcurrencies(subdir,path,pattern,cmp);
			closedir(subdir);
		}
    }

}
