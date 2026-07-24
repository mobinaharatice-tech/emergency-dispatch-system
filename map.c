#include "map.h"
#include "common.h"
#include "config.h"
#include "incident.h"
#include "unit.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


MapCell map[MAP_MAX_HEIGHT][MAP_MAX_WIDTH];
int map_width = 20;
int map_height = 10;


void map_init(int width, int height) {
    map_width = width;
    map_height = height;
    clear_map();
}

void clear_map() {
    for (int y = 0; y < map_height; y++) {
        for (int x = 0; x < map_width; x++) {
            strcpy(map[y][x].symbol, ".");
        }
    }
}


void render_map() {
    for (int y = 0; y < map_height; y++) {
        for (int x = 0; x < map_width; x++) {
            const char* cell = map[y][x].symbol;
            printf(COLOR_RESET);


            if (cell[0] == '!') {
                printf(MAP_INCIDENT);
            }
            else if (strcmp(cell, ".") == 0) {
                printf(MAP_EMPTY);
            }
            else if (strchr(cell, 'F') && !strchr(cell, '-')) {
                printf(MAP_FIRE_DEPT);
            }
            else if (strchr(cell, 'M') && !strchr(cell, '-')) {
                printf(MAP_MED_DEPT);
            }
            else if (strchr(cell, 'P') && !strchr(cell, '-')) {
                printf(MAP_POLICE_DEPT);
            }
            else if (strstr(cell, "F-")) {
                printf(MAP_FIRE_UNIT);
            }
            else if (strstr(cell, "M-")) {
                printf(MAP_MED_UNIT);
            }
            else if (strstr(cell, "P-")) {
                printf(MAP_POLICE_UNIT);
            }

            printf("%-5s", cell);
        }
        printf("\n");
    }
    printf(COLOR_RESET); // Final reset
}


void place_departments() {
    for (int i = 0; i < department_count; i++) {
        const Department* d = &departments[i];
        if (d->x >= 0 && d->x < map_width && d->y >= 0 && d->y < map_height) {
            bool has_resting_unit = false;
            for (int j = 0; j < d->unit_count; j++) {
                if (d->units[j].state == UNIT_RESTING &&
                    d->units[j].x == d->x &&
                    d->units[j].y == d->y) {
                    has_resting_unit = true;
                    break;
                }
            }

            snprintf(map[d->y][d->x].symbol, MAX_SYMBOL_LEN, "%c%d",
                     (d->type == FIRE) ? 'F' :
                     (d->type == MEDICAL) ? 'M' : 'P',
                     d->id);
        }
    }
}

void place_incidents() {
    for (int i = 0; i < incident_count; i++) {
        const Incident* inc = &incidents[i];
        if (inc->state != INCIDENT_FINISHED) {
            strncpy(map[inc->y][inc->x].symbol, inc->symbol, MAX_SYMBOL_LEN);
            map[inc->y][inc->x].symbol[MAX_SYMBOL_LEN-1] = '\0';
        }
    }
}

void place_units() {
    // Clear unit symbols first
    for (int y = 0; y < map_height; y++) {
        for (int x = 0; x < map_width; x++) {
            if (strchr(map[y][x].symbol, '-') != NULL) {
                strcpy(map[y][x].symbol, ".");
            }
        }
    }

    // Place visible units
    for (int i = 0; i < department_count; i++) {
        for (int j = 0; j < departments[i].unit_count; j++) {
            Unit* u = &departments[i].units[j];
            if (u->state != UNIT_OPERATING &&
                !(u->state == UNIT_WAITING &&
                  u->x == departments[i].x &&
                  u->y == departments[i].y) &&
                u->state != UNIT_RESTING) {
                strncpy(map[u->y][u->x].symbol, u->symbol, MAX_SYMBOL_LEN);
                map[u->y][u->x].symbol[MAX_SYMBOL_LEN-1] = '\0';
            }
        }
    }
}


void update_map(void) {
    clear_map();
    place_departments();
    place_incidents();
    place_units();
}

void update_and_render(void) {
    update_map();
    render_map();
}


bool is_incident_cell(int x, int y) {
    for (int i = 0; i < incident_count; i++) {
        if (incidents[i].x == x && incidents[i].y == y &&
            incidents[i].state != INCIDENT_FINISHED) {
            return true;
        }
    }
    return false;
}

void map_clear_incident(int x, int y) {
    if (x >= 0 && x < map_width && y >= 0 && y < map_height) {
        strcpy(map[y][x].symbol, ".");
    }
}


int map_get_width(void) { return map_width; }
int map_get_height(void) { return map_height; }