#include "menu.h"
#include "map.h"
#include "utils.h"
#include "incident.h"
#include "log.h"
#include <stdio.h>
#include "stats.h"

static int current_turn = 0;
static int turns_since_menu = 0;

void menu_clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static int validate_coordinates(int x, int y) {
    return x >= 0 && x < map_width && y >= 0 && y < map_height;
}

void menu_init(void) {
    current_turn = 0;
    turns_since_menu = 0;
    log_event("Menu system initialized");
}

int menu_get_current_turn(void) {
    return current_turn;
}


static void show_welcome(void) {
    printf("\n%s=== Emergency Dispatch Simulator ===%s\n",
           COLOR_CYAN,
           COLOR_RESET);
    printf("%sCurrent turn:%s %d\n",
           COLOR_YELLOW,
           COLOR_RESET,
           current_turn);
}

static void show_turn_header(void) {
    printf("\n\x1b[44;30m--- Turn %d ---\x1b[0m\n", current_turn);
}

static int show_menu_options(void) {
    printf("\n%sMenu Options:%s\n", COLOR_PURPLE, COLOR_RESET);
    printf("%s1. Add New Incident%s\n", COLOR_BLUE, COLOR_RESET);
    printf("%s2. Continue Simulation%s\n", COLOR_GREEN, COLOR_RESET);
    printf("%s3. Simulation Report%s\n", COLOR_YELLOW, COLOR_RESET);
    printf("%s4. Exit%s\n", COLOR_RED, COLOR_RESET);
    printf("%sEnter choice: %s", COLOR_YELLOW, COLOR_RESET);

    int choice;
    char input[10];

    while (1) {
        if (fgets(input, sizeof(input), stdin)) {
            if (sscanf(input, "%d", &choice) == 1) {
                if (choice >= 1 && choice <= 4) {
                    return choice;
                }
            }
        }
        printf("Invalid input! Please enter 1-4: ");
    }
}

void menu_handle_new_incident(void) {
    printf("\n%s-- Create New Incidents --%s\n", COLOR_CYAN, COLOR_RESET);

    int incident_count;
    char input[50];

    // Get number of incidents
    while (1) {
        printf("%sHow many incidents? (1-%d):%s ", COLOR_BLUE, MAX_INCIDENTS - incident_count, COLOR_RESET);
        if (fgets(input, sizeof(input), stdin) && sscanf(input, "%d", &incident_count) == 1) {
            if (incident_count >= 1 && incident_count <= (MAX_INCIDENTS - incident_count)) break;
        }
        printf("%sInvalid number!%s ", COLOR_RED, COLOR_RESET);
    }

    for (int i = 0; i < incident_count; i++) {
        printf("\n%s-- Incident %d --%s\n", COLOR_CYAN, i+1, COLOR_RESET);

        IncidentType type;
        Priority priority;
        int x, y;

        // Get type
        while (1) {
            printf("%sType (1=Fire, 2=Medical, 3=Crime):%s ", COLOR_BLUE, COLOR_RESET);
            if (fgets(input, sizeof(input), stdin) && sscanf(input, "%d", &type) == 1) {
                if (type >= 1 && type <= 3) break;
            }
            printf("%sInvalid type!%s ", COLOR_RED, COLOR_RESET);
        }
        type--;

        // Get priority
        while (1) {
            printf("%sPriority (1=Low, 2=Medium, 3=High):%s ", COLOR_BLUE, COLOR_RESET);
            if (fgets(input, sizeof(input), stdin) && sscanf(input, "%d", &priority) == 1) {
                if (priority >= 1 && priority <= 3) break;
            }
            printf("%sInvalid priority!%s ", COLOR_RED, COLOR_RESET);
        }
        priority--;

        // Get location
        while (1) {
            printf("%sLocation (x y):%s ", COLOR_BLUE, COLOR_RESET);
            if (fgets(input, sizeof(input), stdin) && sscanf(input, "%d %d", &x, &y) == 2) {
                if (validate_coordinates(x, y)) break;
            }
            printf("%sInvalid coordinates!%s ", COLOR_RED, COLOR_RESET);
        }

        // Check unit availability
        if (!can_handle_incident(type, priority)) {
            printf("%sWarning: Not enough available units for this incident!%s\n",
                   COLOR_YELLOW, COLOR_RESET);
            continue;
        }

        add_incident(type, priority, x, y);
        printf("%sAdded %s incident at (%d,%d)%s\n",
               COLOR_GREEN, incident_type_to_str(type), x, y, COLOR_RESET);
    }
}

static void handle_continue_simulation(void) {
    printf("\nPress ENTER to continue to turn %d...", current_turn + 1);
    menu_clear_input_buffer();
    getchar();

    current_turn++;
    turns_since_menu++;

    show_turn_header();
    update_map();
    render_map();


    dispatch_units();
    update_units_movement();
    update_incidents();
}

void menu_main_loop(void) {
    show_welcome();
    update_map();
    render_map();

    while (1) {
        if (turns_since_menu >= MENU_INTERVAL || current_turn == 0) {
            turns_since_menu = 0;
            int choice = show_menu_options();

            switch (choice) {
                case 1:  // Add new incident
                    menu_handle_new_incident();
                    update_map();
                    render_map();
                    break;

                case 2:  // Continue simulation
                    handle_continue_simulation();
                    break;

                case 3:  // Show simulation report
                    stats_print_report();
                    printf("\nPress ENTER to return to menu...");
                    menu_clear_input_buffer();
                    getchar();
                    update_map();
                    render_map();
                    break;

                case 4:  // Exit
                    log_event("Simulation ended by user");
                    printf("\n%sFinal Simulation Report:%s\n", COLOR_CYAN, COLOR_RESET);
                    stats_print_report();
                    printf("\nExiting simulation...\n");
                    return;

                default:
                    printf("%sInvalid choice!%s\n", COLOR_RED, COLOR_RESET);
            }
        } else {
            handle_continue_simulation();
        }
    }
}