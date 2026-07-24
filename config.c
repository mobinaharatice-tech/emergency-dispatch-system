#include "config.h"
#include "map.h"
#include "unit.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>


static bool validate_department(int x, int y, int unit_count);


IncidentType parse_department_type(const char *str) {
    if (strcmp(str, "FIRE") == 0) return FIRE;
    if (strcmp(str, "MEDICAL") == 0) return MEDICAL;
    return POLICE;
}

Priority parse_priority(const char *str) {
    if (strcmp(str, "LOW") == 0) return LOW;
    if (strcmp(str, "MEDIUM") == 0) return MEDIUM;
    return HIGH;
}

void load_configuration(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        log_error("Failed to open configuration file");
        exit(1);
    }

    char line[256];
    bool map_size_loaded = false;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;

        if (strncmp(line, "MAP_SIZE", 8) == 0) {
            if (sscanf(line, "MAP_SIZE %d %d", &map_width, &map_height) != 2) {
                log_error("Invalid MAP_SIZE format");
                continue;
            }
            map_size_loaded = true;
        }
        else if (strncmp(line, "DEPARTMENT", 10) == 0) {
            char type_str[16];
            int id, x, y, units;

            if (sscanf(line, "DEPARTMENT %15s %d %d %d %d",
                       type_str, &id, &x, &y, &units) != 5) {
                log_error("Invalid DEPARTMENT format");
                continue;
            }

            DepartmentType type = parse_department_type(type_str);

            if (!validate_department(x, y, units)) continue;
            if (department_count >= MAX_DEPARTMENTS) {
                log_error("Maximum departments exceeded");
                continue;
            }

            Department *dept = &departments[department_count++];
            dept->type = type;
            dept->id = id;
            dept->x = x;
            dept->y = y;
            dept->unit_count = units;

            snprintf(dept->symbol, sizeof(dept->symbol), "%c%d",
                     (type == FIRE) ? 'F' : (type == MEDICAL) ? 'M' : 'P',
                     id);

            dept->units = malloc(units * sizeof(Unit));
            for (int i = 0; i < units; i++) {
                dept->units[i] = (Unit){
                        .id = i+1,
                        .type = type,
                        .state = UNIT_WAITING,
                        .x = x,
                        .y = y,
                        .target_x = x,
                        .target_y = y,
                        .dept_id = id,
                        .operation_turns = 0
                };
                snprintf(dept->units[i].symbol, sizeof(dept->units[i].symbol),
                         "%s-%d", dept->symbol, i+1);
            }
        }
    }
    fclose(file);

    if (!map_size_loaded) {
        log_error("MAP_SIZE not specified in config");
        exit(1);
    }
    if (department_count == 0) {
        log_error("No departments loaded");
        exit(1);
    }

    map_init(map_width, map_height);
    units_init();
}

static bool validate_department(int x, int y, int unit_count) {
    if (x < 0 || x >= MAP_MAX_WIDTH || y < 0 || y >= MAP_MAX_HEIGHT) {
        log_error("Department coordinates out of bounds");
        return false;
    }
    if (unit_count <= 0 || unit_count > MAX_UNITS_PER_DEPARTMENT) {
        log_error("Invalid unit count for department");
        return false;
    }
    return true;
}

bool config_load(const char* filename) {
    load_configuration(filename);
    return true;
}

void config_cleanup(void) {
    for (int i = 0; i < department_count; i++) {
        if (departments[i].units) {
            free(departments[i].units);
        }
    }
    department_count = 0;
    units_cleanup();
}

const Department* config_get_departments(void) { return departments; }
int config_get_department_count(void) { return department_count; }