#ifndef UNIT_H
#define UNIT_H

#include "common.h"
#include <stdbool.h>


typedef struct Unit Unit;
typedef struct Department Department;


struct Department {
    int x, y;
    DepartmentType type;
    int id;
    char symbol[4];
    int unit_count;
    Unit* units;
};


struct Unit {
    int id;
    DepartmentType type;
    UnitState state;
    int x, y;
    int target_x, target_y;
    int dept_id;
    char symbol[8];
    int operation_turns;
    Department* department;
};


#define MAX_UNITS_PER_DEPT 10



extern Department departments[MAX_DEPARTMENTS];
extern int department_count;
extern Unit** all_units;
extern int total_units;


void units_init(void);
void units_cleanup(void);
void update_units_movement(void);
bool is_cell_occupied(int x, int y, Unit* exclude_unit);
Unit* find_closest_available_unit(int x, int y, DepartmentType type);
void assign_unit_to_incident(Unit* unit, Incident* incident);
void return_unit_to_base(Unit* unit);
void move_unit_one_step(Unit* unit);
bool is_unit_resting(const Unit* unit);
int get_total_units(void);
Unit* get_unit_by_index(int index);


void check_unit_arrival(Unit* unit);
void log_unit_resting(const Unit* unit, int turns_left);
void log_unit_state_transition(const Unit* unit, UnitState old_state, UnitState new_state);
void log_unit_finished_resting(const Unit* unit);

#ifdef UNIT_DEBUG
void print_unit_state(const Unit* unit);
void debug_all_units(void);
#endif

#endif // UNIT_H