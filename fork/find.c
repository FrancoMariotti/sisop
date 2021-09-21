#include <stdio.h>
#include <dirent.h>
#include <string.h>

#define CURRENT_DIRECTORY "."
#define PREVIOUS_DIRECTORY ".."

void printOcurrencies(DIR* folder,char* pattern/*,compare_function*/);

int
main(int argc, char *argv[]) {
    // Your code here
    if (argc < 2) {
		perror("cantidad de parametros incorrecta");
		return -1;
    }

    char* cadena = argv[1];
    DIR* folder = opendir(CURRENT_DIRECTORY);
    if (!folder) {
		perror("error en opendir");
		return -1;
    }

	printOcurrencies(folder,cadena);
	closedir(folder);
    return 0;
}


void printOcurrencies(DIR* folder,char* pattern/*,compare_function*/) {
	struct dirent *entry;
    while ((entry = readdir(folder))) {
		char* ret = strstr(entry->d_name,pattern);

		if (entry->d_type == DT_REG) {
			if (ret) {
				printf("%s\n",entry->d_name);
			}
		} else if (entry->d_type == DT_DIR) {
			if (ret) {
 				printf("directorio: %s\n",entry->d_name);
			}

		}
    }

}
