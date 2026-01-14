#include "calender.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include "fileIO.h"
#include "userInput.h"

static unsigned int id_counter = 0;

// function to make the id's
int generate_id() {
	// if the id limit is reached, it shuts down
	if (id_counter == UINT_MAX) {
		printf("id overflow!\n");
		exit(-1);
	}
	return id_counter++;
}


void init_calendar(calendar_node** root, unsigned short date, unsigned short type) {
	*root = (calendar_node*)malloc(sizeof(calendar_node));
	if (*root == NULL) { exit(-1); }

	(*root)->date_element = date;
	(*root)->type = type;
	(*root)->id = generate_id();

	// setting the pointers to NULL to prevent issues
	(*root)->children = NULL;
	(*root)->siblings = NULL;
	(*root)->data = NULL;
}


// searches for an element with a specific date and type
calendar_node* search_date_element(const calendar_node* root, unsigned short date, unsigned short type) {
	// stop condition
	if (root == NULL) { return NULL; }

	// used to temporarely store to search of the children
	calendar_node* result = NULL;

	// if the type is wrong we move further down the tree
	if (root->type != type) {
		result = search_date_element(root->children, date, type);
	}
	if (root->date_element == date) {
		return root;
	}
	
	return result ? result : search_date_element(root->siblings, date, type);
} 


// search function that searches for criteria defined by the user.
// user must return 1 for found and 0 for not found
calendar_node* key_search(const calendar_node* root, void* to_find,
	int (*key)(const calendar_node*, void*)) {
	// stop condition
	if (root == NULL) { return NULL; }

	// used to temporarely store to search of the children
	calendar_node* result = NULL;

	// search all of the children to
	result = key_search(root->children, to_find, key);

	if (key(root, to_find)) {
		return root;
	}

	// when result is NULL it searches the other siblings and their children
	return result ? result : key_search(root->siblings, to_find, key);
}


// works the same as key search but adds all the results to a list
static int full_search_helper(calendar_node* root, void* to_find,
	calendar_node*** result, int index,
	int (*key)(const calendar_node*, void*)) {
	// stop condition
	if (root == NULL) { return index; }

	// search all the siblings
	index = full_search_helper(root->siblings, to_find, result, index, key);

	// search all of the children to
	index = full_search_helper(root->children, to_find, result, index, key);

	// because the tree is built in a sorted manner
	// and we first move all the way to the back of the tree
	// the result will contain all matches in chronologic order

	// checking with the key
	if (key(root, to_find)) {
		// adding the found node to the list
		*result = realloc(*result, sizeof(calendar_node*) * (index + 1));
		if (*result == NULL) { exit(-1); }

		(*result)[index++] = root;
	}

	return index;
}


// like key_search but this one makes a dynamic list of all the results
// returns number of found elements
int full_key_search(calendar_node* root, void* to_find,
	calendar_node*** result,
	int (*key)(const calendar_node*, void*)) {
	// making a dynamic list for the results
	*result = malloc(sizeof(calendar_node*));
	if (*result == NULL) { exit(-1); }
	return full_search_helper(root, to_find, result, 0, key);
}


// searches for a node inside a specific pathway (year, month, day)
calendar_node* search_full_date(const calendar_node* root, unsigned short* pathway, unsigned short len) {
	calendar_node* path = root;
	// moves trough the calendar using path
	// if an element in pathway doesn't exist path will be NULL
	for (size_t i = 0; i < len; i++) {
		path = search_date_element(path, pathway[i], i);
	}

	return path;
}


// adds a sibling sorted by date
void add_sibling(calendar_node** root, calendar_node* child, calendar_node* prev) {
	// stopping condition
	if (*root == NULL) {
		// if prev is NULL the tree is empty
		if (prev != NULL) { prev->siblings = child; }
		else { 
			*root = child;
		}
		return;
	}

	// making sure the types are the same
	if ((*root)->type != child->type) {
		printf("types must be the same, nothing added\n");
		return;
	}

	// comparing dates and adding if possible
	if ((*root)->date_element > child->date_element) {
		// checking if prev is NULL
		if (prev == NULL) {
			// add using root
			child->siblings = *root;
			*root = child;
		}
		else {
			// normal add (linked list)
			child->siblings = prev->siblings;
			prev->siblings = child;
		}
		return;
	}

	add_sibling(&(*root)->siblings, child, *root);
}

// adds an element to the calendar, but also adds the nodes that lead
// to that element if they don't exist already.
// pathway is used to get the rigth year, month, etc
void full_add(calendar_node** root, calendar_node* child, unsigned short pathway[3]) {
	size_t index = 0;
	calendar_node* next = NULL;
	calendar_node** path = root;

	// moving to the rigth level
	while (true) {
		// checking when we can stop
		if (index != child->type) {
			// checking if the path is NULL
			if (*path == NULL) {
				// making the path
				calendar_node* to_add = NULL;
				init_calendar(&to_add, pathway[index], index);	// index also moves along the types
				add_sibling(path, to_add, NULL);
			}

			// checking if the path already exist
			if ((next = search_date_element(*path, pathway[index], index)) == NULL) {
				// making the path
				calendar_node* to_add = NULL;
				init_calendar(&to_add, pathway[index], index);	// index also moves along the types
				add_sibling(path, to_add, NULL);
				next = to_add;
			}

			path = &next->children;	// not *path to not overwrite the root
			index++;
		}
		else {
			break;
		}
	}

	// now that path is of the rigth type, we add the child
	add_sibling(path, child, NULL);
}


static int get_parent_key(const calendar_node* root, void* child) {
	calendar_node* compare = (calendar_node*)child;
	if (root->siblings && root->siblings->id == compare->id) {
		return 1;
	}

	if (root->children && root->children->id == compare->id) {
		return 1;
	}

	return 0;
}


// function to find the parent of a node
calendar_node* get_parent(const calendar_node* root, const calendar_node* child) {
	return key_search(root, child, &get_parent_key);
}


// frees the calendar in the heap
void free_calendar(calendar_node* root, calendar_node** to_free, calendar_node* parent) {
	calendar_node* cur_node = *to_free;

	// moving trough the tree
	if (cur_node->siblings) {
		free_calendar(root, &cur_node->siblings, cur_node);
	}
	if (cur_node->children) {
		free_calendar(root, &cur_node->children, cur_node);
	}

	// making sure the parent doesn't have acces to the freed child anymore
	if (parent) {
		// cur_node can either be a child or a sibling
		if (parent->type == cur_node->type) {
			parent->siblings = NULL;
		}
		else {
			parent->children = NULL;
		}
	}

	free_event_extension(cur_node->data, cur_node);
	free(cur_node);
	*to_free = NULL;
}


static int check_id_key(calendar_node* root, void* to_find) {
	calendar_node* compare = (calendar_node*)to_find;
	if (root->id == compare->id) {
		return 1;
	}
	return 0;
}


// deletes a calendar_node
void delete_node(calendar_node** root, calendar_node* node) {
	calendar_node* to_delete;
	// checking if the node even exists
	if (!(to_delete = key_search(*root, node, &check_id_key))) {
		printf("the node you tried to delete doesn't exist in the calendar\n");
		return;
	}

	calendar_node* parent = get_parent(*root, to_delete);
	// if the parent is null that means to_delete is the head of the calendar
	if (parent) {
		// to_delete can either be a sibling or a child
		if (parent->type == to_delete->type) {
			calendar_node* new_siblings = to_delete->siblings;
			free_calendar(*root, &to_delete, parent);
			parent->siblings = new_siblings;
		}
		else {
			calendar_node* new_children = to_delete->siblings;
			to_delete->siblings = NULL;		// to prevent the siblings from getting deleted
			free_calendar(*root, &to_delete, parent);
			parent->children = new_children;
		}
	}
	else {
		calendar_node* new_head = to_delete->siblings;
		to_delete->siblings = NULL;	// to prevent the siblings from getting freed
		free_calendar(*root, &to_delete, NULL);
		*root = new_head;
	}
}


void init_event_extension(event_extension** event, char date[11], char start[6], char end[6]) {
	*event = (event_extension*)malloc(sizeof(event_extension));
	if (*event == NULL) { exit(-1); }

	strcpy((*event)->date, date);
	strcpy((*event)->start, start);
	strcpy((*event)->end, end);
	(*event)->id = generate_id();

	// making room for the title etc
	(*event)->title = (char*)malloc(sizeof(char) * MAX_TITLE);
	(*event)->description = (char*)malloc(sizeof(char) * MAX_DESCRIPTION);
	(*event)->location = (char*)malloc(sizeof(char) * MAX_LOCATION);

	if (!(*event)->title || !(*event)->description || !(*event)->location) {
		exit(-1);
	}

	strcpy((*event)->title, "no title");
	strcpy((*event)->description, "no description");
	strcpy((*event)->location, "no location");
}


void free_event_extension(event_extension* event, calendar_node* parent) {
	if (event) {
		if (parent) { parent->data = NULL; }
		free(event->title);
		free(event->description);
		free(event->location);
		free(event);
	}
}


// writes a calendar to a specified file
void export_full_calendar(const calendar_node* root, char* path) {
	open_filepath(path, "w");
	export_calendar_node(root);
	close_filepath();
}


// writes a calendar_node to a specified file
void export_calendar_node(const calendar_node* root) {
	char buffer[300];

	if (root == NULL) {
		write_var("NULL");
		return;
	}

	start_element();

	sprintf(buffer, "%d", root->date_element);
	write_var(buffer);

	// id doesn't get exported because we generate new ones

	sprintf(buffer, "%d", root->type);
	write_var(buffer);

	export_event_extension(root->data);

	export_calendar_node(root->siblings);
	export_calendar_node(root->children);

	end_element();
}


// writes an event to a specified file
void export_event_extension(const event_extension* event) {
	char buffer[300];

	if (event == NULL) {
		write_var("NULL");
		return;
	}

	start_element();

	// id doesn't get exported because we generate new ones

	write_var(event->date);
	write_var(event->start);
	write_var(event->end);
	write_var(event->title);
	write_var(event->description);
	write_var(event->location);

	end_element();
}


// helper function to turn text into numbers
static int convert_to_int(char* string) {
	int number;
	sscanf(string, "%d", &number);
	return number;
}


// strips \n from the string
static char* strip(char* string) {
	char buffer[300] = "";
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
	strcpy(string, buffer);

	return string;
}


// reads the specified file and makes the full calendar
calendar_node* import_full_calendar(char* path) {
	open_filepath(path, "r");
	calendar_node* root = import_calendar_node();
	close_filepath();

	return root;
}


// reads the specified file and makes a calendar_node
calendar_node* import_calendar_node() {
	char buffer[300];

	// if there is no data or file is empty
	if (!readline(buffer) || !strcmp(strip(buffer), "NULL")) {
		return NULL;
	}

	calendar_node* imported = NULL;
	init_calendar(&imported, 0, 0);

	imported->date_element = convert_to_int(strip(readline(buffer)));
	imported->type = convert_to_int(strip(readline(buffer)));

	imported->data = import_event_extension();
	imported->siblings = import_calendar_node();
	imported->children = import_calendar_node();

	readline(buffer);		// to read the closing bracket

	return imported;
}


// reads the specified file and makes an event
event_extension* import_event_extension() {
	char buffer[300];

	// if there is no data
	if (!strcmp(strip(readline(buffer)), "NULL")) {
		return NULL;
	}

	event_extension* imported = NULL;
	init_event_extension(&imported, "", "", "");

	strcpy(buffer, strip(readline(buffer)));
	strcpy(imported->date, buffer);

	strcpy(buffer, strip(readline(buffer)));
	strcpy(imported->start, buffer);

	strcpy(buffer, strip(readline(buffer)));
	strcpy(imported->end, buffer);

	strcpy(buffer, strip(readline(buffer)));
	strcpy(imported->title, buffer);

	strcpy(buffer, strip(readline(buffer)));
	strcpy(imported->description, buffer);

	strcpy(buffer, strip(readline(buffer)));
	strcpy(imported->location, buffer);

	readline(buffer);		// to read the closing bracket

	return imported;
}


// this makes comparisons much easier
static int date_to_int(char* date) {
	int day, month, year;
	sscanf(date, "%d/%d/%d", &day, &month, &year);
	return year * 10000 + month * 100 + day;
}


// this makes comparisons much easier
static int time_to_int(char* time) {
	int hours, minutes;
	sscanf(time, "%d:%d", &hours, &minutes);
	return hours * 60 + minutes;
}


// converts the date into an int and check if it's within range
static int date_range_key(calendar_node* root, void* parameters) {
	// {min_date_value, max_date_value}
	int* dates = (int*)parameters;

	// if it has an event extension
	if (root->data) {
		// checking if date is in range
		int root_value = date_to_int(root->data->date);

		if (root_value >= dates[0] && root_value <= dates[1]) {
			return 1;
		}
	}

	return 0;
}


void plan_appointment(calendar_node** root) {
	char buffer[BUFFER_SIZE];
	char date[11], start[6], end[6];

	// getting the user input
	get_date(buffer, BUFFER_SIZE, "geef een datum: ");
	strcpy(date, buffer);

	get_time(buffer, BUFFER_SIZE, "geef een start tijd: ");
	strcpy(start, buffer);

	get_time(buffer, BUFFER_SIZE, "geef een eind tijd: ");
	strcpy(end, buffer);

	// initiating the event
	event_extension* event = NULL;
	init_event_extension(&event, date, start, end);

	get_text(buffer, BUFFER_SIZE, "geef een titel: ");
	if (strcmp(buffer, "")) {
		strncpy(event->title, buffer, MAX_TITLE);
	}

	get_text(buffer, BUFFER_SIZE, "geef een beschrijving: ");
	if (strcmp(buffer, "")) {
		strncpy(event->description, buffer, MAX_DESCRIPTION);
	}

	get_text(buffer, BUFFER_SIZE, "geef een locatie: ");
	if (strcmp(buffer, "")) {
		strncpy(event->location, buffer, MAX_LOCATION);
	}

	// the date element is calculated using starting time
	// this way it is sorted earliest to latest
	calendar_node* to_add = NULL;
	init_calendar(&to_add, time_to_int(start), EVENT);
	to_add->data = event;

	// the pathway contains the date values leading to the event
	int pathway[3];
	sscanf(date, "%d/%d/%d", &pathway[2], &pathway[1], &pathway[0]);
	pathway[1] -= 1;	// to get the rigth index for months
	full_add(root, to_add, pathway);
}


// deletes all the events in a certain date range
void delete_range(calendar_node** root) {
	char buffer[BUFFER_SIZE];
	char start_date[11];
	char end_date[11];

	// getting user input
	get_date(buffer, BUFFER_SIZE, "geef een begin datum: ");
	strcpy(start_date, buffer);

	get_date(buffer, BUFFER_SIZE, "geef een eind datum: ");
	strcpy(end_date, buffer);

	int parameters[] = {
		date_to_int(start_date),
		date_to_int(end_date)
	};

	// searching for all the nodes with a date in this range
	calendar_node** result = NULL;
	int n_results = full_key_search(
		*root, parameters,
		&result, &date_range_key
	);

	for (size_t i = 0; i < n_results; i++) {
		delete_node(root, result[i]);
	}

	// contents of result were freed during for loop
	free(result);
}


// prints all the events in a certain date range
void print_range(calendar_node** root) {
	char buffer[BUFFER_SIZE];
	char start_date[11];
	char end_date[11];

	// getting the user input
	get_date(buffer, BUFFER_SIZE, "geef een begin datum: ");
	strcpy(start_date, buffer);

	get_date(buffer, BUFFER_SIZE, "geef een eind datum: ");
	strcpy(end_date, buffer);

	int parameters[] = {
		date_to_int(start_date),
		date_to_int(end_date)
	};

	// searching for all the nodes with a date in this range
	calendar_node** result = NULL;
	int n_results = full_key_search(
		*root, parameters,
		&result, &date_range_key
	);

	// we iterate in reverse in order to put the results chronologicly
	for (int i = n_results - 1; i >= 0; i--) {
		printf("--------------------------------------------------------------------------------------------------\n");

		printf("titel: ");
		printf("%s\n", result[i]->data->title);

		printf("datum: ");
		printf("%s\n", result[i]->data->date);

		printf("tijd: ");
		printf("%s - %s\n", result[i]->data->start, result[i]->data->end);

		printf("beschrijving: ");
		printf("%s\n", result[i]->data->description);

		printf("locatie: ");
		printf("%s\n", result[i]->data->location);

		printf("--------------------------------------------------------------------------------------------------\n");
	}

	// contents of result don't need to be freed since they
	// are still in the tree
	free(result);
}


static int is_event_key(calendar_node* root, void* nothing) {
	if (root->data) {
		return 1;
	}
	return 0;
}


// prints all the events in a calendar
void print_full_calendar(calendar_node** root) {
	calendar_node** result = NULL;
	int n_results = full_key_search(
		*root, NULL,
		&result, &is_event_key
	);

	// we iterate in reverse in order to put the results chronologicly
	for (int i = n_results - 1; i >= 0; i--) {
		printf("--------------------------------------------------------------------------------------------------\n");

		printf("titel: ");
		printf("%s\n", result[i]->data->title);

		printf("datum: ");
		printf("%s\n", result[i]->data->date);

		printf("tijd: ");
		printf("%s - %s\n", result[i]->data->start, result[i]->data->end);

		printf("beschrijving: ");
		printf("%s\n", result[i]->data->description);

		printf("locatie: ");
		printf("%s\n", result[i]->data->location);

		printf("--------------------------------------------------------------------------------------------------\n");
	}

	// contents of result don't need to be freed since they
	// are still in the tree
	free(result);
}


static int minimum(int a, int b) {
	return a < b ? a : b;
}


// compares the charakters of the search string with the title of the event
// if more then half of the charakters match, it's a true match
static int textual_match_key(calendar_node* root, void* text) {
	char* compare = (char*)text;
	if (root->data) {
		int matches = 0;
		char* root_text = root->data->title;

		// iterate over the strings and check if the chars are the same
		int limit = minimum(strlen(root_text), strlen(compare));
		for (size_t i = 0; i < limit; i++) {
			if (root_text[i] == compare[i]) {
				matches++;
			}
		}

		if (matches > strlen(compare) / 2) {
			return 1;
		}
	}

	return 0;
}


void search_textual_match(calendar_node** root) {
	char buffer[BUFFER_SIZE];
	get_text(buffer, BUFFER_SIZE, "zoekstring: ");

	calendar_node** result = NULL;
	int n_results = full_key_search(
		*root, buffer,
		&result, &textual_match_key
	);

	// we iterate in reverse in order to put the results chronologicly
	for (int i = n_results - 1; i >= 0; i--) {
		printf("--------------------------------------------------------------------------------------------------\n");

		printf("titel: ");
		printf("%s\n", result[i]->data->title);

		printf("datum: ");
		printf("%s\n", result[i]->data->date);

		printf("tijd: ");
		printf("%s - %s\n", result[i]->data->start, result[i]->data->end);

		printf("beschrijving: ");
		printf("%s\n", result[i]->data->description);

		printf("locatie: ");
		printf("%s\n", result[i]->data->location);

		printf("--------------------------------------------------------------------------------------------------\n");
	}

	// contents of result don't need to be freed since they
	// are still in the tree
	free(result);
}