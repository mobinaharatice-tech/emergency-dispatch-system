#ifndef SIMULATION_H
#define SIMULATION_H

#include "common.h"
#include <stdbool.h>

#define MENU_INTERVAL 10  // Show menu every 10 turns


void simulation_init(void);
void simulation_loop(void);
void simulation_cleanup(void);


void process_simulation_turn(void);
void show_menu_after_interval(void);


int get_current_turn(void);
bool is_simulation_running(void);

static void process_unit_movements(void);
static void check_incident_progress(void);
static void process_user_input(int choice);
static void perform_turn_actions(void);
static void handle_unit_arrival(Unit* u);
static void handle_unit_return(Unit* u);
static void handle_resting_units(Unit* u);
static void resolve_completed_incidents(Incident* inc);

#endif // SIMULATION_H