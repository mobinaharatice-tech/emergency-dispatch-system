#ifndef INCIDENT_H
#define INCIDENT_H

#include "common.h"
#include "unit.h"
#include <stdbool.h>
#include <stdlib.h>

// Incident requirements structure
typedef struct {
    int fire_units;
    int medical_units;
    int police_units;
    int turns_required;
} IncidentRequirement;

typedef struct Incident {
    // Identification and location
    int id;
    int x, y;

    // Incident properties
    IncidentType type;
    Priority priority;
    IncidentState state;

    // Timing and operation
    int operation_turns_remaining;
    int turns_remaining;

    // Unit management
    Unit *assigned_units[DEPT_TYPE_COUNT][MAX_UNITS_PER_DEPARTMENT];
    int assigned_counts[DEPT_TYPE_COUNT];

    // Display
    char symbol[8];
} Incident;


#define MAX_INCIDENTS 100
#define MAX_REQUIRED_UNITS_PER_TYPE 5
#define MAX_UNITS_PER_DEPT 10  // Should match your common.h


extern Incident *incidents;
extern int incident_count;
extern int incident_capacity;
extern const IncidentRequirement requirements[INCIDENT_TYPE_COUNT][PRIORITY_COUNT];

void init_incidents(void);
void add_incident(IncidentType type, Priority priority, int x, int y);
void dispatch_units(void);
void update_incidents(void);
void resolve_incident(Incident* incident);
void ensure_incident_capacity(void);


const char* incident_get_symbol(IncidentType type, Priority priority);
bool incident_needs_more_units(const Incident* inc);
bool can_handle_incident(IncidentType type, Priority priority);

#endif // INCIDENT_H