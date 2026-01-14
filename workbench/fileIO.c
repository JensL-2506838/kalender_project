#include "fileIO.h"
#include <string.h>
#include <stdio.h>


static FILE* filepath = NULL;
static char* filemode = "r";
void open_filepath(const char* path, char* mode) {
	filepath = fopen(path, mode);

	// checking if file exists
	if (!filepath) {
		printf("file does not exist\n");
		return;
	}

	filemode = mode;
}


void close_filepath() {
	fclose(filepath);
}


void start_element() {
	if (!strcmp(filemode, "w") && filepath) {
		fputs("{\n", filepath);
	}
}


void end_element() {
	if (!strcmp(filemode, "w") && filepath) {
		fputs("}\n", filepath);
	}
}


void write_var(const char* var) {
	if (!strcmp(filemode, "w") && filepath) {
		fputs(var, filepath);
		fputs("\n", filepath);
	}
}


char* readline(char* buffer) {
	if (!strcmp(filemode, "r") && filepath) {
		if (fgets(buffer, 300, filepath) != NULL) {
			return buffer;
		}
	}
	return NULL;
}
