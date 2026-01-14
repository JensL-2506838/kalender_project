#include "fileIO.h"
#include <string.h>
#include <stdio.h>


static FILE* filepath = NULL;
static char* filemode = "r";
void open_filepath(const char* path, char* mode) {
	filepath = fopen(path, mode);
	filemode = mode;
}


void close_filepath() {
	fclose(filepath);
}


void start_element() {
	if (!strcmp(filemode, "w")) {
		fputs("{\n", filepath);
	}
}


void end_element() {
	if (!strcmp(filemode, "w")) {
		fputs("}\n", filepath);
	}
}


void write_var(const char* var) {
	if (!strcmp(filemode, "w")) {
		fputs(var, filepath);
		fputs("\n", filepath);
	}
}


char* readline(char* buffer) {
	if (!strcmp(filemode, "r")) {
		if (fgets(buffer, 300, filepath) != NULL) {
			return buffer;
		}
	}
	return NULL;
}
