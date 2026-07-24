#include "incident.h"
#include "unit.h"
#include "map.h"
#include "log.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stats.h"
Incident *incidents = NULL;
int incident_count = 0;
int incident_capacity = 0;
static SimulationStats stats = {0};
const IncidentRequirement requirements[INCIDENT_TYPE_COUNT][PRIORITY_COUNT] = {
        [INCIDENT_FIRE] = {
                [PRIORITY_LOW]    = {1, 0, 0, 4},
                [PRIORITY_MEDIUM] = {1, 1, 0, 7},
                [PRIORITY_HIGH]   = {1, 1, 1, 9}
        },
        [INCIDENT_MEDICAL] = {
                [PRIORITY_LOW]    = {0, 1, 0, 2},
                [PRIORITY_MEDIUM] = {0, 1, 0, 4},
                [PRIORITY_HIGH]   = {0, 1, 0, 6}
        },
        [INCIDENT_CRIME] = {
                [PRIORITY_LOW]    = {0, 0, 1, 3},
                [PRIORITY_MEDIUM] = {0, 1, 1, 5},
                [PRIORITY_HIGH]   = {0, 1, 1, 9}
        }
};

const char* incident_get_symbol(IncidentType type, Priority priority) {
    static const char* symbols[INCIDENT_TYPE_COUNT][PRIORITY_COUNT] = {
            [INCIDENT_FIRE] = {
                    [PRIORITY_LOW] = "!FL!",
                    [PRIORITY_MEDIUM] = "!FM!",
                    [PRIORITY_HIGH] = "!FH!"
            },
            [INCIDENT_MEDICAL] = {
                    [PRIORITY_LOW] = "!ML!",
                    [PRIORITY_MEDIUM] = "!MM!",
                    [PRIORITY_HIGH] = "!MH!"
            },
            [INCIDENT_CRIME] = {
                    [PRIORITY_LOW] = "!CL!",
                    [PRIORITY_MEDIUM] = "!CM!",
                    [PRIORITY_HIGH] = "!CH!"
            }
    };
    return symbols[type][priority];
}

void init_incidents(void) {
    if (incidents) {
        free(incidents);
    }
    incident_capacity = 10; // capacity
    incidents = malloc(incident_capacity * sizeof(Incident));
    incident_count = 0;
    memset(&stats, 0, sizeof(stats));
}

void ensure_incident_capacity() {
    if (incident_count >= incident_capacity) {
        incident_capacity *= 2; // Double capacity
        incidents = realloc(incidents, incident_capacity * sizeof(Incident));
        if (!incidents) {
            log_error("Failed to reallocate incident memory");
            exit(1);
        }
    }
}

void add_incident(IncidentType type, Priority priority, int x, int y) {
    ensure_incident_capacity();

    stats_incident_created();
    Incident* inc = &incidents[incident_count++];

    *inc = (Incident){
            .id = incident_count,
            .x = x,
            .y = y,
            .type = type,
            .priority = priority,
            .state = INCIDENT_WAITING,
            .turns_remaining = requirements[type][priority].turns_required,
            .assigned_counts = {0}
    };

    strncpy(inc->symbol, incident_get_symbol(type, priority), sizeof(inc->symbol));
    memset(inc->assigned_units, 0, sizeof(inc->assigned_units));

    log_incident_created(type, priority, x, y);
    printf("%sAdded %s %s incident at (%d,%d)%s\n",
           COLOR_GREEN, priority_to_str(priority),
           incident_type_to_str(type), x, y, COLOR_RESET);
}

bool incident_needs_more_units(const Incident* inc) {
    const IncidentRequirement* req = &requirements[inc->type][inc->priority];
    return (inc->assigned_counts[DEPT_FIRE] < req->fire_units) ||
           (inc->assigned_counts[DEPT_MEDICAL] < req->medical_units) ||
           (inc->assigned_counts[DEPT_POLICE] < req->police_units);
}

void dispatch_units(void) {
    printf("\n==== DISPATCHING UNITS ====\n");
    for (int i = 0; i < incident_count; i++) {
        Incident* inc = &incidents[i];
        if (inc->state != INCIDENT_WAITING) continue;

        printf("Checking incident at (%d,%d)\n", inc->x, inc->y);
        const IncidentRequirement* req = &requirements[inc->type][inc->priority];
        bool logged_fire_unavailable = false;
        bool logged_medical_unavailable = false;
        bool logged_police_unavailable = false;

        // Fire units
        while (inc->assigned_counts[DEPT_FIRE] < req->fire_units) {
            Unit* unit = find_closest_available_unit(inc->x, inc->y, DEPT_FIRE);
            if (!unit) {
                if (!logged_fire_unavailable) {
                    char log_msg[100];
                    snprintf(log_msg, sizeof(log_msg),
                             "No available FIRE units found for incident at (%d,%d)",
                             inc->x, inc->y);
                    log_event(log_msg);
                    printf("No available fire units found!\n");
                    logged_fire_unavailable = true;
                }
                break;
            }
            printf("Dispatching %s to (%d,%d)\n", unit->symbol, inc->x, inc->y);
            assign_unit_to_incident(unit, inc);
            inc->assigned_units[DEPT_FIRE][inc->assigned_counts[DEPT_FIRE]++] = unit;
        }

        // Medical units
        while (inc->assigned_counts[DEPT_MEDICAL] < req->medical_units) {
            Unit* unit = find_closest_available_unit(inc->x, inc->y, DEPT_MEDICAL);
            if (!unit) {
                if (!logged_medical_unavailable) {
                    char log_msg[100];
                    snprintf(log_msg, sizeof(log_msg),
                             "No available MEDICAL units found for incident at (%d,%d)",
                             inc->x, inc->y);
                    log_event(log_msg);
                    printf("No available medical units found!\n");
                    logged_medical_unavailable = true;
                }
                break;
            }
            printf("Dispatching %s to (%d,%d)\n", unit->symbol, inc->x, inc->y);
            assign_unit_to_incident(unit, inc);
            inc->assigned_units[DEPT_MEDICAL][inc->assigned_counts[DEPT_MEDICAL]++] = unit;
        }

        // Police units
        while (inc->assigned_counts[DEPT_POLICE] < req->police_units) {
            Unit* unit = find_closest_available_unit(inc->x, inc->y, DEPT_POLICE);
            if (!unit) {
                if (!logged_police_unavailable) {
                    char log_msg[100];
                    snprintf(log_msg, sizeof(log_msg),
                             "No available POLICE units found for incident at (%d,%d)",
                             inc->x, inc->y);
                    log_event(log_msg);
                    printf("No available police units found!\n");
                    logged_police_unavailable = true;
                }
                break;
            }
            printf("Dispatching %s to (%d,%d)\n", unit->symbol, inc->x, inc->y);
            assign_unit_to_incident(unit, inc);
            inc->assigned_units[DEPT_POLICE][inc->assigned_counts[DEPT_POLICE]++] = unit;
        }

        if (!incident_needs_more_units(inc)) {
            inc->state = INCIDENT_OPERATION;
            printf("Incident at (%d,%d) now in operation\n", inc->x, inc->y);
        }
    }
    printf("==== DISPATCH COMPLETE ====\n\n");
}

void update_incidents(void) {
    for (int i = 0; i < incident_count; i++) {
        Incident* inc = &incidents[i];

        if (inc->state != INCIDENT_OPERATION) continue;

        // Check if ALL required units are present
        bool all_units_present = true;
        const IncidentRequirement* req = &requirements[inc->type][inc->priority];

        // Count present units
        int fire_present = 0, medical_present = 0, police_present = 0;

        for (int j = 0; j < inc->assigned_counts[DEPT_FIRE]; j++) {
            Unit* u = inc->assigned_units[DEPT_FIRE][j];
            if (u->x == inc->x && u->y == inc->y) {
                fire_present++;
            }
        }
        for (int j = 0; j < inc->assigned_counts[DEPT_MEDICAL]; j++) {
            Unit* u = inc->assigned_units[DEPT_MEDICAL][j];
            if (u->x == inc->x && u->y == inc->y) {
                medical_present++;
            }
        }
        for (int j = 0; j < inc->assigned_counts[DEPT_POLICE]; j++) {
            Unit* u = inc->assigned_units[DEPT_POLICE][j];
            if (u->x == inc->x && u->y == inc->y) {
                police_present++;
            }
        }


        if (fire_present < req->fire_units ||
            medical_present < req->medical_units ||
            police_present < req->police_units) {
            all_units_present = false;
        }

        if (all_units_present) {

            inc->turns_remaining--;

            char log_msg[100];
            snprintf(log_msg, sizeof(log_msg),
                     "Incident %d (%s) operation in progress: %d turns remaining",
                     inc->id, inc->symbol, inc->turns_remaining);
            log_event(log_msg);

            if (inc->turns_remaining <= 0) {
                // Operation complete - return units and resolve incident
                for (int type = 0; type < DEPT_TYPE_COUNT; type++) {
                    for (int j = 0; j < inc->assigned_counts[type]; j++) {
                        Unit* u = inc->assigned_units[type][j];
                        if (u && u->x == inc->x && u->y == inc->y) {
                            return_unit_to_base(u);
                        }
                    }
                }
                resolve_incident(inc);
            }
        }
    }
}


void resolve_incident(Incident* inc) {
    if (!inc) return;
    log_event("========================================");

    int turns_taken = requirements[inc->type][inc->priority].turns_required - inc->turns_remaining;

    // Update statistics
    stats_incident_solved(inc);

    // Mark as finished and return units
    inc->state = INCIDENT_FINISHED;
    log_incident_finished(inc->type, inc->x, inc->y);

    // Return all assigned units to their departments
    for (int type = 0; type < DEPT_TYPE_COUNT; type++) {
        for (int j = 0; j < inc->assigned_counts[type]; j++) {
            Unit* u = inc->assigned_units[type][j];
            if (u && u->state == UNIT_OPERATING) {
                return_unit_to_base(u);
            }
        }
    }

    // Clear the incident from the map
    map_clear_incident(inc->x, inc->y);


    char log_msg[150];
    snprintf(log_msg, sizeof(log_msg),
             "Incident %d (%s) resolved at (%d,%d) in %d turns",
             inc->id, incident_type_to_str(inc->type),
             inc->x, inc->y, turns_taken - 1);
    log_event(log_msg);
    log_event("========================================");
}

bool can_handle_incident(IncidentType type, Priority priority) {
    const IncidentRequirement* req = &requirements[type][priority];
    int available_fire = 0, available_medical = 0, available_police = 0;

    // Count available units
    for (int i = 0; i < total_units; i++) {
        Unit* u = all_units[i];
        if (u->state == UNIT_WAITING) {
            switch (u->type) {
                case DEPT_FIRE: available_fire++; break;
                case DEPT_MEDICAL: available_medical++; break;
                case DEPT_POLICE: available_police++; break;
            }
        }
    }

    // Check if requirements can be met
    return (available_fire >= req->fire_units) &&
           (available_medical >= req->medical_units) &&
           (available_police >= req->police_units);
}