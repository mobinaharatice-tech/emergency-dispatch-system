#include "stats.h"
#include <stdio.h>
#include "incident.h"
static SimulationStats current_stats = {0};

static int get_expected_turns(IncidentType type, Priority priority) {

    static const int expected_turns[INCIDENT_TYPE_COUNT][PRIORITY_COUNT] = {
            [INCIDENT_FIRE] = {
                    [PRIORITY_LOW] = 3,
                    [PRIORITY_MEDIUM] = 6,
                    [PRIORITY_HIGH] = 8
            },
            [INCIDENT_MEDICAL] = {
                    [PRIORITY_LOW] = 1,
                    [PRIORITY_MEDIUM] = 3,
                    [PRIORITY_HIGH] = 5
            },
            [INCIDENT_CRIME] = {
                    [PRIORITY_LOW] = 2,
                    [PRIORITY_MEDIUM] = 4,
                    [PRIORITY_HIGH] = 8
            }
    };
    return expected_turns[type][priority];
}

void stats_init(void) {
    current_stats.solved_incidents = 0;
    current_stats.unsolved_incidents = 0;
    current_stats.active_incidents = 0;
    current_stats.total_response_time = 0;
}
void stats_incident_solved(Incident* inc) {
    current_stats.solved_incidents++;

    current_stats.total_response_time += get_expected_turns(inc->type, inc->priority);
    if (current_stats.active_incidents > 0) {
        current_stats.active_incidents--;
    }
}
void stats_incident_created(void) {
    current_stats.active_incidents++;
}
void stats_incident_active(int count) {
    current_stats.active_incidents = count;
}
SimulationStats stats_get_current(void) {
    return current_stats;
}
void stats_print_report(void) {

    printf("\n%s+", COLOR_YELLOW);
    printf("%s====================== SIMULATION REPORT ======================%s", COLOR_CYAN, COLOR_YELLOW);
    printf("+%s\n", COLOR_RESET);


    printf("%s|%s %-25s %s|%s %-33s %s|%s\n",
           COLOR_YELLOW, COLOR_RESET, "topic",
           COLOR_YELLOW, COLOR_RESET, "Value",
           COLOR_YELLOW, COLOR_RESET);


    printf("%s+", COLOR_YELLOW);
    printf("%s---------------------------%s", COLOR_CYAN, COLOR_YELLOW);
    printf("+");
    printf("%s-----------------------------------%s", COLOR_CYAN, COLOR_YELLOW);
    printf("+%s\n", COLOR_RESET);


    printf("%s|%s %s%-25s%s %s|%s %s%-33d%s %s|%s\n",
           COLOR_YELLOW, COLOR_RESET,
           COLOR_PURPLE, "Solved Incidents", COLOR_RESET,
           COLOR_YELLOW, COLOR_RESET,
           COLOR_PURPLE, current_stats.solved_incidents, COLOR_RESET,
           COLOR_YELLOW, COLOR_RESET);

    printf("%s|%s %s%-25s%s %s|%s %s%-33d%s %s|%s\n",
           COLOR_YELLOW, COLOR_RESET,
           COLOR_PURPLE, "Unsolved Incidents", COLOR_RESET,
           COLOR_YELLOW, COLOR_RESET,
           COLOR_PURPLE, current_stats.unsolved_incidents, COLOR_RESET,
           COLOR_YELLOW, COLOR_RESET);

    printf("%s|%s %s%-25s%s %s|%s %s%-33d%s %s|%s\n",
           COLOR_YELLOW, COLOR_RESET,
           COLOR_PURPLE, "Active Incidents", COLOR_RESET,
           COLOR_YELLOW, COLOR_RESET,
           COLOR_PURPLE, current_stats.active_incidents, COLOR_RESET,
           COLOR_YELLOW, COLOR_RESET);


    printf("%s+", COLOR_YELLOW);
    printf("%s---------------------------%s", COLOR_CYAN, COLOR_YELLOW);
    printf("+");
    printf("%s-----------------------------------%s", COLOR_CYAN, COLOR_YELLOW);
    printf("+%s\n", COLOR_RESET);


    printf("%s|%s %s%-25s%s %s|%s %s%-33d%s %s|%s\n",
           COLOR_YELLOW, COLOR_RESET,
           COLOR_PURPLE, "Total Operation Time", COLOR_RESET,
           COLOR_YELLOW, COLOR_RESET,
           COLOR_PURPLE, current_stats.total_response_time, COLOR_RESET,
           COLOR_YELLOW, COLOR_RESET);

    int avg_time = current_stats.solved_incidents > 0 ?
                   current_stats.total_response_time / current_stats.solved_incidents : 0;

    printf("%s|%s %s%-25s%s %s|%s %s%-33d%s %s|%s\n",
           COLOR_YELLOW, COLOR_RESET,
           COLOR_PURPLE, "Average Response Time", COLOR_RESET,
           COLOR_YELLOW, COLOR_RESET,
           COLOR_PURPLE, avg_time, COLOR_RESET,
           COLOR_YELLOW, COLOR_RESET);


    printf("%s+", COLOR_YELLOW);
    printf("%s---------------------------------------------------------------%s", COLOR_CYAN, COLOR_YELLOW);
    printf("+%s\n", COLOR_RESET);
}