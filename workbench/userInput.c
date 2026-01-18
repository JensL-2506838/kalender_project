// naam: Jens Laurijssen	Nummer: 2506838
#include "userInput.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>


// strips \n from the string
static char* strip(char* string, const size_t size) {
	char buffer[MAX_BUFFER] = "";
	size_t index = 0;		// to keep track of where to add to the buffer
	// loop trough the string and compare each charakter
	for (size_t i = 0; i < strlen(string); i++) {
		if (string[i] != '\n') {
			// add to the buffer and (post) increment the index
			buffer[index++] = string[i];
		}
	}

	// put result back into the string
	buffer[index] = '\0';
	strncpy(string, buffer, size);

	return string;
}


// empties the stdin the buffer has overflowed
static void handle_overflow(const char* buffer) {
	// if the last char of the input isn't \n
	// it's likely an overflow
	// then we need to remove whatever is left in the stdin
	if (buffer[strlen(buffer) - 1] != '\n') {
		// keep reading till nothing is left
		while (getchar() != '\n') { NULL; }
	}
}


// asks the user for a date in the format (DD/MM/YYYY)
char* get_date(char* buffer, const int size, const char* message) {
	while (1) {
		printf("%s", message);

		// if error ask again
		if (!fgets(buffer, size, stdin)) {
			continue;
		}

		handle_overflow(buffer);

		// if input is empty ask again
		if (!strcmp(buffer, "\n")) {
			continue;
		}

		// get rid of the \n
		strip(buffer, size);

		// checking if nothing is added behind the date
		const char last_char = buffer[strlen(buffer) - 1];
		if (!isdigit(last_char)) {
			printf("Datum moet dit formaat hebben: DD/MM/YYYY\n");
			continue;
		}

		// check the format and extract the data
		int day, month, year;
		if (sscanf(buffer, "%d/%d/%d", &day, &month, &year) != 3) {
			printf("Datum moet dit formaat hebben: DD/MM/YYYY\n");
			continue;
		}

		// check if date is valid
		if (month < 1 || month > 12 ||
			day < 1 || day > days_in_month[month - 1] ||
			year < 0 || year > 9999) {
			printf("Datum moet bestaan\n");
			continue;
		}

		return buffer;
	}
}


// asks for a time of the format (HH:MM)
char* get_time(char* buffer, const int size, const char* message) {
	while (1) {
		printf("%s", message);

		// if error ask again
		if (!fgets(buffer, size, stdin)) {
			continue;
		}

		handle_overflow(buffer);

		// if input is empty ask again
		if (!strcmp(buffer, "\n")) {
			continue;
		}

		// get rid of the \n
		strip(buffer, size);

		// checking if nothing is added behind the time
		const char last_char = buffer[strlen(buffer) - 1];
		if (!isdigit(last_char)) {
			printf("Tijd moet dit formaat hebben: HH:MM\n");
			continue;
		}

		// check the format and extract the data
		int hour, minute;
		if (sscanf(buffer, "%d:%d", &hour, &minute) != 2) {
			printf("Tijd moet dit formaat hebben: HH:MM\n");
			continue;
		}

		// check if the time is valid
		if (hour < 0 || hour >= 24 ||
			minute < 0 || minute >= 60) {
			printf("Tijd moet bestaan\n");
			continue;
		}

		return buffer;
	}
}


// just gets a piece of text from the user
char* get_text(char* buffer, const int size, const char* message) {
	// keep asking if input incorrect
	while (1) {
		printf("%s", message);

		// if correct input break out of loop
		if (fgets(buffer, size, stdin)) {
			handle_overflow(buffer);
			strip(buffer, size);
			break;
		}

		printf("an error occured while taking the input\n");
	}

	return buffer;
}


// asks for a time of the format (HH:MM)
int get_num(const char* message) {
	char buffer[MAX_BUFFER];

	while (1) {
		printf("%s", message);

		// if error ask again
		if (!fgets(buffer, MAX_BUFFER, stdin)) {
			continue;
		}

		handle_overflow(buffer);

		// if input is empty ask again
		if (!strcmp(buffer, "\n")) {
			continue;
		}

		// get rid of the \n
		strip(buffer, MAX_BUFFER);

		// converting to int
		int result;
		if (sscanf(buffer, "%d", &result) != 1) {
			printf("input moet een nummer zijn\n");
			continue;
		}

		return result;
	}
}

