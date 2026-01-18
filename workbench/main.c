// naam: Jens Laurijssen	Nummer: 2506838
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "calendar.h"
#include "userInput.h"


int main(void) {
    calendar_node* root = NULL;

    // interface
    while (true) {
        // printing options
        printf("[0] stop programma\n");
        printf("[1] plan afspraak\n");
        printf("[2] afspraken verwijderen\n");
        printf("[3] leegmaken kalender\n");
        printf("[4] uitschrijven kalender\n");
        printf("[5] afspraken uitschrijven\n");
        printf("[6] afspraken zoeken\n");
        printf("[7] importeer kalender\n");
        printf("[8] exporteer kalender\n\n");

        // getting command
        const int command = get_num("wat wilt u uitvoeren?: ");
        // executing the rigth function
        switch (command) {
            case 0:
                printf("programma sluiten...\n");
                free_calendar(root, &root, NULL);
                return 0;
            case 1:
                plan_appointment(&root);
                break;
            case 2:
                delete_range(&root);
                break;
            case 3:
                free_calendar(root, &root, NULL);
                break;
            case 4:
                print_full_calendar(&root);
                break;
            case 5:
                print_range(&root);
                break;
            case 6:
                search_textual_match(&root);
                break;
            case 7:
                // adds the imported calendar to calendar that already exists
                user_import_calendar(&root);
                break;
            case 8:
                user_export_calendar(&root);
                break;
            default:
                printf("commando bestaat niet\n");
                break;
        }
    }
}
