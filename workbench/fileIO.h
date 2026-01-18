// naam: Jens Laurijssen	Nummer: 2506838
#ifndef fileIO
#define fileIO


void open_filepath(const char* path, char* mode);
void close_filepath();
void start_element();
void end_element();
void write_var(const char* var);
char* readline(char* buffer);


#endif