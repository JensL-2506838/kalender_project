#ifndef fileIO
#define fileIO


void open_filepath(char* path, char* mode);
void close_filepath();
void start_element();
void end_element();
void write_var(char* var);
char* readline(char* buffer);


#endif