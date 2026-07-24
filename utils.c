#include "utils.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

const char* getPriorityStr(Priority p) {
    switch (p) {
        case PRIORITY_LOW: return "LOW";
        case PRIORITY_MEDIUM: return "MEDIUM";
        case PRIORITY_HIGH: return "HIGH";
        default: return "UNKNOWN";
    }
}

const char* getIncidentTypeStr(IncidentType t) {
    switch (t) {
        case INCIDENT_FIRE: return "FIRE";
        case INCIDENT_MEDICAL: return "MEDICAL";
        case INCIDENT_CRIME: return "CRIME";
        default: return "UNKNOWN";
    }
}

const char* getUnitTypeStr(DepartmentType t) {
    switch (t) {
        case DEPT_FIRE: return "FIRE";
        case DEPT_MEDICAL: return "MEDICAL";
        case DEPT_POLICE: return "POLICE";
        default: return "UNKNOWN";
    }
}

const char* getUnitStateStr(UnitState s) {
    switch (s) {
        case UNIT_WAITING: return "WAITING";
        case UNIT_DISPATCHED: return "DISPATCHED";
        case UNIT_OPERATING: return "OPERATING";
        case UNIT_RETURNING: return "RETURNING";
        default: return "UNKNOWN";
    }
}

const char* getIncidentStateStr(IncidentState s) {
    switch (s) {
        case INCIDENT_WAITING: return "WAITING";
        case INCIDENT_OPERATION: return "OPERATION";
        case INCIDENT_FINISHED: return "FINISHED";
        default: return "UNKNOWN";
    }
}

#include "utils.h"

// Incident type to string
const char* incident_type_to_str(IncidentType type) {
    switch(type) {
        case INCIDENT_FIRE: return "FIRE";
        case INCIDENT_MEDICAL: return "MEDICAL";
        case INCIDENT_CRIME: return "CRIME";
        default: return "UNKNOWN";
    }
}

// Priority to string
const char* priority_to_str(Priority priority) {
    switch(priority) {
        case PRIORITY_LOW: return "LOW";
        case PRIORITY_MEDIUM: return "MEDIUM";
        case PRIORITY_HIGH: return "HIGH";
        default: return "UNKNOWN";
    }
}

// Unit type to string
const char* unit_type_to_str(DepartmentType type) {
    switch(type) {
        case DEPT_FIRE: return "FIRE";
        case DEPT_MEDICAL: return "MEDICAL";
        case DEPT_POLICE: return "POLICE";
        default: return "UNKNOWN";
    }
}


bool is_valid_position(int x, int y) {
    return x >= 0 && x < MAP_MAX_WIDTH && y >= 0 && y < MAP_MAX_HEIGHT;
}

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}