#include "simulation.h"
#include "config.h"
#include "incident.h"
#include "unit.h"
#include "map.h"
#include "log.h"
#include "menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static int current_turn = 0;
static int turns_since_menu = 0;
static bool simulation_active = false;

void simulation_loop() {
    simulation_active = true;
    current_turn = 0;
    turns_since_menu = 0;
    log_event("Simulation started");
    render_map();

    // Main game loop
    while (simulation_active) {
        if (turns_since_menu >= MENU_INTERVAL || current_turn == 0) {
            turns_since_menu = 0;
            show_menu_after_interval();
        } else {
            perform_turn_actions();
        }
    }

    log_event("Simulation ended");
}

static void process_user_input(int choice) {
    switch (choice) {
        case 1:  // Add new incident
            menu_handle_new_incident();
            update_map();
            render_map();
            break;

        case 2:  // Continue simulation
            perform_turn_actions();
            break;

        case 3:  // Exit
            simulation_active = false;
            log_event("User exited simulation");
            break;
    }
}


static void perform_turn_actions() {
    current_turn++;
    turns_since_menu++;

    log_event("========================================");
    char turn_msg[100];
    snprintf(turn_msg, sizeof(turn_msg), "TURN %d STARTED", current_turn);
    log_event(turn_msg);

    // 1. Dispatch units to incidents
    dispatch_units();

    // 2. Process unit movements
    process_unit_movements();

    // 3. Update incidents
    check_incident_progress();

    // 4. Update and render map
    update_map();
    render_map();

    printf("\nPress ENTER to continue to turn %d...", current_turn + 1);
    fflush(stdout);
    while (getchar() != '\n');
}


static void process_unit_movements(void) {
    for (int dept_type = DEPT_FIRE; dept_type <= DEPT_POLICE; dept_type++) {
        for (int i = 0; i < total_units; i++) {
            Unit* u = all_units[i];
            if (u->type != dept_type) continue;

            // Movement logic for dispatched/returning units
            if (u->state == UNIT_DISPATCHED || u->state == UNIT_RETURNING) {
                int dx = 0, dy = 0;
                if (u->x != u->target_x) {
                    dx = (u->x < u->target_x) ? 1 : -1;
                } else if (u->y != u->target_y) {
                    dy = (u->y < u->target_y) ? 1 : -1;
                }

                if ((dx != 0 || dy != 0) && !is_cell_occupied(u->x + dx, u->y + dy, u)) {
                    u->x += dx;
                    u->y += dy;
                    log_unit_movement(u);
                }

                // Check if reached destination
                if (u->x == u->target_x && u->y == u->target_y) {
                    if (u->state == UNIT_DISPATCHED) {
                        handle_unit_arrival(u);
                    } else {
                        handle_unit_return(u);
                    }
                }
            }
                // Handle resting units
            else if (u->state == UNIT_RESTING) {
                handle_resting_units(u);
            }
        }
    }
}


static void check_incident_progress(void) {
    for (int i = 0; i < incident_count; i++) {
        Incident* inc = &incidents[i];
        if (inc->state == INCIDENT_OPERATION) {
            const IncidentRequirement* req = &requirements[inc->type][inc->priority];
            int operating_units = 0;

            for (int type = 0; type < DEPT_TYPE_COUNT; type++) {
                for (int j = 0; j < inc->assigned_counts[type]; j++) {
                    if (inc->assigned_units[type][j]->state == UNIT_OPERATING) {
                        operating_units++;
                    }
                }
            }

            if (operating_units >= (req->fire_units + req->medical_units + req->police_units)) {
                inc->turns_remaining--;
                if (inc->turns_remaining <= 0) {
                    resolve_completed_incidents(inc);
                }
            }
        }
    }
}


void show_menu_after_interval(void) {
    int choice;
    do {
        printf("\n=== Simulation Menu (Turn %d) ===\n", current_turn);
        printf("1. Add New Incident\n");
        printf("2. Continue Simulation\n");
        printf("3. Exit\n");
        printf("Select option: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            choice = -1;
        }
    } while (choice < 1 || choice > 3);

    process_user_input(choice);
}


static void handle_unit_arrival(Unit* u) {
    u->state = UNIT_OPERATING;
    log_event("================ ARRIVAL ================");
    char msg[150];
    snprintf(msg, sizeof(msg), "%s ARRIVED at (%d,%d)", u->symbol, u->x, u->y);
    log_event(msg);
    log_event("========================================");
}

static void handle_unit_return(Unit* u) {
    u->state = UNIT_RESTING;
    u->operation_turns = 3;
    log_event("=============== RETURNED HOME ===============");
    char msg[150];
    snprintf(msg, sizeof(msg), "%s RETURNED to base", u->symbol);
    log_event(msg);
    log_event("========================================");
}

static void handle_resting_units(Unit* u) {
    u->operation_turns--;
    if (u->operation_turns <= 0) {
        u->state = UNIT_WAITING;
        char msg[100];
        snprintf(msg, sizeof(msg), "%s READY for dispatch", u->symbol);
        log_event(msg);
    }
}

static void resolve_completed_incidents(Incident* inc) {
    for (int type = 0; type < DEPT_TYPE_COUNT; type++) {
        for (int j = 0; j < inc->assigned_counts[type]; j++) {
            Unit* u = inc->assigned_units[type][j];
            if (u->state == UNIT_OPERATING) {
                return_unit_to_base(u);
            }
        }
    }
    inc->state = INCIDENT_FINISHED;
    log_event("============ INCIDENT RESOLVED ============");
    char msg[150];
    snprintf(msg, sizeof(msg), "INCIDENT %d RESOLVED at (%d,%d)", inc->id, inc->x, inc->y);
    log_event(msg);
    log_event("========================================");
}