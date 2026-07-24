#ifndef STATS_H
#define STATS_H

#include "common.h"

// Statistics structure
typedef struct {
    int solved_incidents;
    int unsolved_incidents;
    int active_incidents;
    int total_response_time;
} SimulationStats;


void stats_init(void);


void stats_incident_solved(Incident* inc);
void stats_incident_created(void);
void stats_incident_active(int count);


SimulationStats stats_get_current(void);


void stats_print_report(void);

#endif // STATS_H