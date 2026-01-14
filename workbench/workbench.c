#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calender.h"
#include "fileIO.h"
#include "userInput.h"


int temp_key(const calendar_node* root, void* to_find) {
	if (root->type == (unsigned short)to_find) {
		return 1;
	}
	return 0;
}


int main(void) {
	calendar_node* root = import_full_calendar("C:\\Users\\jensl\\OneDrive\\Desktop\\input.txt");

	//plan_appointment(&root);
	//delete_range(&root);
	//print_range(&root);
	//print_full_calendar(&root);
	search_textual_match(&root);

	export_full_calendar(root, "C:\\Users\\jensl\\OneDrive\\Desktop\\output.txt");

	free_calendar(root, &root, NULL);

	if (!root) {
		printf("root freed\n");
	}

	return 0;
}
