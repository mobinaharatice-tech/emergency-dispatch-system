#ifndef LOG_H
#define LOG_H

#include "common.h"
#include "unit.h"
#include <stdio.h>

extern FILE *log_file;

/* Initialization */
void log_init(void);
void log_close(void);
void init_log(void);


void log_simulation_start(void);
void log_simulation_end(void);


void log_incident_created(IncidentType type, Priority priority, int x, int y);
void log_incident_finished(IncidentType type, int x, int y);
void log_incident_operation_start(int x, int y, IncidentType type);
void log_incident_operation_progress(int x, int y, int turns_remaining);
void log_incident_status_change(int x, int y, IncidentState state, IncidentType type);

/* Unit logging - merged both versions */
void log_unit_dispatched(Unit *u, int target_x, int target_y);
void log_unit_dispatched_const(const Unit* unit, int target_x, int target_y);
void log_unit_state_change(Unit *u, UnitState state);
void log_unit_state_change_const(const Unit* unit, UnitState new_state);
void log_unit_arrived(Unit *u, int incident_x, int incident_y);
void log_unit_arrived_const(const Unit* unit, int incident_x, int incident_y);
void log_unit_returning(Unit *u, int base_x, int base_y);
void log_unit_returned(const Unit* unit);

void log_unit_state_transition(const Unit* unit, UnitState old_state, UnitState new_state);
void log_unit_resting(const Unit* unit, int turns_left);
void log_unit_finished_resting(const Unit* unit);


void log_event(const char* message);
void log_error(const char* message);

void log_unit_movement(const Unit* unit);

#define log_unit_dispatched(u, x, y) log_unit_dispatched_const(u, x, y)
#define log_unit_state_change(u, s) log_unit_state_change_const(u, s)
#define log_unit_arrived(u, x, y) log_unit_arrived_const(u, x, y)

#endif // LOG_H