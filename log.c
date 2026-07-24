#include <direct.h>
#include "log.h"
#include "common.h"
#include "unit.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

FILE *log_file = NULL;

static void log_timestamp() {
    time_t now;
    time(&now);
    struct tm *tm = localtime(&now);
    fprintf(log_file, "[%04d-%02d-%02d %02d:%02d:%02d] ",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);
}

void log_init(void) {
    if (log_file) return;

    time_t now;
    time(&now);
    struct tm *tm = localtime(&now);
    char filename[LOG_FILENAME_MAX_LEN];

    strftime(filename, sizeof(filename), LOG_FILENAME_PREFIX "%Y%m%d_%H%M%S" LOG_FILENAME_EXT, tm);
    printf("Creating log file: %s\n", filename);

    log_file = fopen(filename, "a");
    if (!log_file) {
        perror("ERROR: Failed to create log file");
        exit(EXIT_FAILURE);
    }
}

void init_log(void) {
    log_init();
}

void log_close(void) {
    if (log_file) {
        log_simulation_end();
        fclose(log_file);
        log_file = NULL;
    }
}

void log_simulation_start(void) {
    if (!log_file) return;

    time_t now;
    time(&now);
    log_timestamp();
    fprintf(log_file, "SIMULATION STARTED at %s", ctime(&now));
    fflush(log_file);
}

void log_simulation_end(void) {
    if (!log_file) return;
    log_timestamp();
    fprintf(log_file, "SIMULATION FINISHED\n");
    fflush(log_file);
}

void log_incident_created(IncidentType type, Priority priority, int x, int y) {
    if (!log_file) return;
    log_timestamp();
    fprintf(log_file, "Incident Created: Type=%s, Priority=%s, Location=(%d,%d)\n",
            type == INCIDENT_FIRE ? "FIRE" :
            type == INCIDENT_MEDICAL ? "MEDICAL" : "CRIME",
            priority == PRIORITY_HIGH ? "HIGH" :
            priority == PRIORITY_MEDIUM ? "MEDIUM" : "LOW",
            x, y);
    fflush(log_file);
}

void log_incident_finished(IncidentType type, int x, int y) {
    if (!log_file) return;
    log_timestamp();
    fprintf(log_file, "Incident Finished: Type=%s, Location=(%d,%d)\n",
            type == INCIDENT_FIRE ? "FIRE" :
            type == INCIDENT_MEDICAL ? "MEDICAL" : "CRIME",
            x, y);
    fflush(log_file);
}

void log_incident_operation_start(int x, int y, IncidentType type) {
    if (!log_file) return;
    log_timestamp();
    fprintf(log_file, "Incident Operation Started: Type=%s, Location=(%d,%d)\n",
            type == INCIDENT_FIRE ? "FIRE" :
            type == INCIDENT_MEDICAL ? "MEDICAL" : "CRIME",
            x, y);
    fflush(log_file);
}

void log_incident_operation_progress(int x, int y, int turns_remaining) {
    if (!log_file) return;
    log_timestamp();
    fprintf(log_file, "Incident Progress: Location=(%d,%d), Turns Remaining=%d\n",
            x, y, turns_remaining);
    fflush(log_file);
}

void log_incident_status_change(int x, int y, IncidentState state, IncidentType type) {
    if (!log_file) return;
    log_timestamp();
    fprintf(log_file, "Incident at (%d,%d) [Type=%s] state changed to %s\n",
            x, y,
            type == INCIDENT_FIRE ? "FIRE" :
            type == INCIDENT_MEDICAL ? "MEDICAL" : "CRIME",
            state == INCIDENT_WAITING ? "WAITING" :
            state == INCIDENT_OPERATION ? "OPERATION" : "FINISHED");
    fflush(log_file);
}


void log_unit_dispatched(const Unit* unit, int target_x, int target_y) {
    if (!log_file || !unit) return;
    log_timestamp();
    fprintf(log_file, "Unit Dispatched: %s, Target=(%d,%d)\n",
            unit->symbol, target_x, target_y);
    fflush(log_file);
}

void log_unit_state_change(const Unit* unit, UnitState new_state) {
    if (!log_file || !unit) return;
    log_timestamp();
    fprintf(log_file, "Unit State Change: %s -> %s\n",
            unit->symbol,
            new_state == UNIT_WAITING ? "WAITING" :
            new_state == UNIT_DISPATCHED ? "DISPATCHED" :
            new_state == UNIT_OPERATING ? "OPERATING" : "RETURNING");
    fflush(log_file);
}

void log_unit_arrived(const Unit* unit, int incident_x, int incident_y) {
    if (!log_file || !unit) return;
    log_timestamp();
    fprintf(log_file, "Unit Arrived: %s at Incident (%d,%d)\n",
            unit->symbol, incident_x, incident_y);
    fflush(log_file);
}

void log_unit_returned(const Unit* unit) {
    if (!log_file || !unit) return;
    log_timestamp();
    fprintf(log_file, "Unit Returned: %s to Department\n", unit->symbol);
    fflush(log_file);
}

void log_unit_returning(Unit *u, int base_x, int base_y) {
    if (!log_file || !u) return;
    log_timestamp();
    fprintf(log_file, "Unit %s returning to HOME at (%d,%d)\n",
            u->symbol, base_x, base_y);
    fflush(log_file);
}

void log_unit_movement(const Unit* unit) {
    if (!log_file || !unit) return;

    log_timestamp();
    fprintf(log_file, "Unit %s moved to (%d,%d)\n",
            unit->symbol, unit->x, unit->y);
    fflush(log_file);
}


void log_event(const char* message) {
    if (!log_file || !message) return;
    log_timestamp();
    fprintf(log_file, "Event: %s\n", message);
    fflush(log_file);
}

void log_error(const char* message) {
    if (!log_file || !message) return;
    log_timestamp();
    fprintf(log_file, "ERROR: %s\n", message);
    fprintf(stderr, "ERROR: %s\n", message);
    fflush(log_file);
}

void log_unit_state_transition(const Unit* unit, UnitState old_state, UnitState new_state) {
    if (!log_file || !unit) return;
    log_timestamp();
    fprintf(log_file, "Event: %s STATE CHANGE: %s → %s\n",
            unit->symbol,
            (old_state == UNIT_WAITING) ? "WAITING" :
            (old_state == UNIT_DISPATCHED) ? "DISPATCHED" :
            (old_state == UNIT_OPERATING) ? "OPERATING" :
            (old_state == UNIT_RETURNING) ? "RETURNING" : "RESTING",
            (new_state == UNIT_WAITING) ? "WAITING" :
            (new_state == UNIT_DISPATCHED) ? "DISPATCHED" :
            (new_state == UNIT_OPERATING) ? "OPERATING" :
            (new_state == UNIT_RETURNING) ? "RETURNING" : "RESTING");
    fflush(log_file);
}

void log_unit_resting(const Unit* unit, int turns_left) {
    if (!log_file || !unit) return;
    log_timestamp();
    fprintf(log_file, "Event: Unit %s resting (%d turns left)\n",
            unit->symbol, turns_left);
    fflush(log_file);
}

void log_unit_finished_resting(const Unit* unit) {
    if (!log_file || !unit) return;
    log_timestamp();
    fprintf(log_file, "Event: Unit %s is now WAITING (finished resting)\n",
            unit->symbol);
    fflush(log_file);
}