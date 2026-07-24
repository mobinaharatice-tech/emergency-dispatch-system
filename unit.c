#include "unit.h"
#include "config.h"
#include "incident.h"
#include "map.h"
#include "log.h"
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>


Unit** all_units = NULL;
int total_units = 0;
Department departments[MAX_DEPARTMENTS];
int department_count = 0;

int manhattan_distance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

void move_unit_one_step(Unit *u) {
    if (!u || (u->state != UNIT_DISPATCHED && u->state != UNIT_RETURNING)) {
        return;
    }

    // Calculate direction
    int dx = 0, dy = 0;
    if (u->x != u->target_x) {
        dx = (u->x < u->target_x) ? 1 : -1;
    } else if (u->y != u->target_y) {
        dy = (u->y < u->target_y) ? 1 : -1;
    }

    // Move if path is clear
    if ((dx != 0 || dy != 0) && !is_cell_occupied(u->x + dx, u->y + dy, u)) {
        u->x += dx;
        u->y += dy;
    }

    // Check for arrival
    if (u->x == u->target_x && u->y == u->target_y) {
        if (u->state == UNIT_DISPATCHED) {
            u->state = UNIT_OPERATING;
            log_unit_arrived(u, u->target_x, u->target_y);
        } else if (u->state == UNIT_RETURNING) {
            u->state = UNIT_RESTING;
            u->operation_turns = 3;
            log_unit_returning(u, u->x, u->y);
        }
    }
}

void update_units_movement() {
    for (int i = 0; i < total_units; i++) {
        Unit* u = all_units[i];

        // Handle moving units
        if (u->state == UNIT_DISPATCHED || u->state == UNIT_RETURNING) {
            move_unit_one_step(u);
        }

        else if (u->state == UNIT_RESTING) {

            u->operation_turns--;

            // Log resting status
            log_unit_resting(u, u->operation_turns);

            // Check if rest period is over
            if (u->operation_turns <= 0) {
                UnitState old_state = u->state;
                u->state = UNIT_WAITING;

                // Log state change
                log_unit_state_transition(u, old_state, u->state);

                // Log completion
                log_unit_finished_resting(u);
            }
        }

        // Check if unit reached destination
        check_unit_arrival(u);
    }
}

/* Your original functions (preserved exactly) */
void units_init(void) {
    // Count total units
    total_units = 0;
    for (int i = 0; i < department_count; i++) {
        total_units += departments[i].unit_count;
    }

    // Create array of unit pointers
    all_units = malloc(total_units * sizeof(Unit*));
    int idx = 0;
    for (int i = 0; i < department_count; i++) {
        for (int j = 0; j < departments[i].unit_count; j++) {
            all_units[idx++] = &departments[i].units[j];
        }
    }

    // Verify unit-department assignments
    for (int i = 0; i < total_units; i++) {
        Unit* u = all_units[i];
        bool found = false;
        for (int j = 0; j < department_count; j++) {
            if (departments[j].id == u->dept_id &&
                departments[j].type == u->type) {
                found = true;
                break;
            }
        }
        if (!found) {
            char error_msg[100];
            snprintf(error_msg, sizeof(error_msg), "Unit %s has invalid department reference!", u->symbol);
            log_error(error_msg);
        }
    }
}

void units_cleanup(void) {
    if (all_units) free(all_units);
    all_units = NULL;
    total_units = 0;
}

Unit* find_closest_available_unit(int x, int y, DepartmentType type) {
    Unit* closest = NULL;
    int min_dist = INT_MAX;

    for (int i = 0; i < total_units; i++) {
        Unit* u = all_units[i];
        if (u->type == type &&
            u->state == UNIT_WAITING &&
            u->operation_turns <= 0) {
            int dist = manhattan_distance(x, y, u->x, u->y);
            if (dist < min_dist) {
                min_dist = dist;
                closest = u;
            }
        }
    }
    return closest;
}

void assign_unit_to_incident(Unit* unit, Incident* incident) {
    if (!unit || !incident) return;
    UnitState old_state = unit->state;
    unit->target_x = incident->x;
    unit->target_y = incident->y;
    unit->state = UNIT_DISPATCHED;
    log_unit_state_transition(unit, old_state, unit->state);
    log_unit_dispatched(unit, incident->x, incident->y);
}

void check_unit_arrival(Unit* unit) {
    if (!unit) return;

    if (unit->x == unit->target_x && unit->y == unit->target_y) {
        UnitState old_state = unit->state; // ADD THIS LINE

        if (unit->state == UNIT_DISPATCHED) {
            unit->state = UNIT_OPERATING;
            log_unit_state_transition(unit, old_state, unit->state);
            log_event("========================================");
            log_unit_arrived(unit, unit->target_x, unit->target_y);
        }
        else if (unit->state == UNIT_RETURNING) {
            unit->state = UNIT_RESTING;
            unit->operation_turns = 3;
            log_unit_state_transition(unit, old_state, unit->state);
            log_event("========================================");

            // ADD THESE LINES:
            char msg[150];
            snprintf(msg, sizeof(msg), "%s ARRIVED at %c%d (RESTING for 3 turns)",
                     unit->symbol,
                     (unit->type == DEPT_FIRE) ? 'F' :
                     (unit->type == DEPT_MEDICAL) ? 'M' : 'P',
                     unit->dept_id);
            log_event(msg);
        }
    }
}

void return_unit_to_base(Unit* unit) {
    if (!unit) return;

    for (int i = 0; i < department_count; i++) {
        if (departments[i].id == unit->dept_id &&
            departments[i].type == unit->type) {

            unit->target_x = departments[i].x;
            unit->target_y = departments[i].y;
            unit->state = UNIT_RETURNING;
            log_unit_returning(unit, departments[i].x, departments[i].y);
            return;
        }
    }

    char error_msg[150];
    snprintf(error_msg, sizeof(error_msg),
             "ERROR: Can't find home department for %s (Type: %d, DeptID: %d)",
             unit->symbol, unit->type, unit->dept_id);
    log_error(error_msg);
}

bool is_cell_occupied(int x, int y, Unit* exclude_unit) {
    if (x < 0 || x >= map_width || y < 0 || y >= map_height) {
        return true;
    }

    for (int i = 0; i < total_units; i++) {
        Unit* u = all_units[i];
        if (u != exclude_unit && u->x == x && u->y == y &&
            (u->state == UNIT_DISPATCHED || u->state == UNIT_RETURNING)) {
            return true;
        }
    }

    return false;
}

void debug_all_units() {
    printf("\n--- UNIT STATES ---\n");
    for (int i = 0; i < department_count; i++) {
        printf("Department %c%d:\n",
               (departments[i].type == DEPT_FIRE) ? 'F' :
               (departments[i].type == DEPT_MEDICAL) ? 'M' : 'P',
               departments[i].id);

        for (int j = 0; j < departments[i].unit_count; j++) {
            Unit* u = &departments[i].units[j];
            printf("  %s at (%d,%d) state: %s\n",
                   u->symbol, u->x, u->y,
                   (u->state == UNIT_WAITING) ? "WAITING" :
                   (u->state == UNIT_DISPATCHED) ? "DISPATCHED" :
                   (u->state == UNIT_OPERATING) ? "OPERATING" :
                   (u->state == UNIT_RETURNING) ? "RETURNING" : "RESTING");
        }
    }
}