#ifndef userInput
#define userInput
#define MAX_BUFFER 1000

static int days_in_month[] = {
	31,	// januari
	29,	// februari
	31, // ...
	30,
	31,
	30,
	31,
	31,
	30,
	31,
	30,
	31
};


// asks the user for a date in the format (DD/MM/YYYY)
char* get_date(char* buffer, int size, const char* message);

// asks for a time of the format (HH:MM)
char* get_time(char* buffer, int size, const char* message);

// just gets a piece of text from the user
char* get_text(char* buffer, int size, const char* message);


#endif