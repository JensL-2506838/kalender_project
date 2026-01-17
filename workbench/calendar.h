#ifndef CALENDAR_H
#define CALENDAR_H

#define MAX_TITLE 100
#define MAX_DESCRIPTION 300
#define MAX_LOCATION 100

#define BUFFER_SIZE 300

// used to track wich element it is
enum calendar_type {YEAR, MONTH, DAY, EVENT};
enum months {JANUARY, FEBRUARY, MARCH, APRIL, MAY, JUNE, JULY,
			AUGUST, SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER};


// this is used to add extra data when a calendar element is an event
typedef struct event_extension {
	unsigned int id;
	char date[11];	// we assume a length of 8 because of the font (DD:MM:YYYY)\0
	char start[6];	// we assume a length of 5 because of the font (HH:MM)\0
	char end[6];	// we assume a length of 5 because of the font (HH:MM)\0
	char* title;
	char* description;
	char* location;
} event_extension;


// this node is used to build the calendar
typedef struct calendar_node {
	unsigned short date_element;	// this can be transformed into months via lists
	unsigned int id;							// this is given to all nodes so i can track them
	unsigned short type;			// year, month, day, event
	event_extension* data;			// used when type is event
	struct calendar_node* siblings;
	struct calendar_node* children;
} calendar_node;


// function to make the id's
unsigned int generate_id();


void init_calendar(calendar_node** root, unsigned short date, unsigned short type);


// searches for an element with a specific date and type
calendar_node* search_date_element(calendar_node* root, unsigned short date, unsigned short type);

// search function that searches for criteria defined by the user.
// user must return 1 for found and 0 for not found
calendar_node* key_search(calendar_node* root, void* to_find,
						int (*key)(const calendar_node*, void*));


// like key_search but this one makes a dynamic list of all the results
// returns number of found elements
int full_key_search(calendar_node* root, void* to_find,
	calendar_node*** result,
	int (*key)(const calendar_node*, void*));


// searches for a node inside a specific pathway (year, month, day)
calendar_node* search_full_date(calendar_node* root, const unsigned short* pathway, unsigned short len);


// adds a sibling sorted by date
void add_sibling(calendar_node** root, calendar_node* child, calendar_node* prev);

// adds an element to the calendar, but also adds the nodes that lead
// to that element if they don't exist already
void full_add(calendar_node** root, calendar_node* child, int pathway[3]);


// function to find the parent of a node
calendar_node* get_parent(calendar_node* root, calendar_node* child);


// frees the calendar in the heap
void free_calendar(calendar_node* root, calendar_node** to_free, calendar_node* parent);


// deletes a node
void delete_node(calendar_node** root, calendar_node* node);


void init_event_extension(event_extension** event, char date[11], char start[6], char end[6]);


void free_event_extension(event_extension* event, calendar_node* parent);


void export_full_calendar(calendar_node* root, const char* path);
void export_calendar_node(const calendar_node* root);


calendar_node* import_full_calendar(calendar_node** root, const char* path);
calendar_node* import_calendar_node();


// integrates the functions defined above to interact with the user
void plan_appointment(calendar_node** root);
void delete_range(calendar_node** root);
void print_range(calendar_node** root);
void print_full_calendar(calendar_node** root);
void search_textual_match(calendar_node** root);
void user_import_calendar(calendar_node** root);
void user_export_calendar(calendar_node** root);
void import_calendar(calendar_node** root);
void export_calendar(calendar_node* root);
void print_event(const event_extension* event);


#endif
