#ifndef UTILS_H
#define UTILS_H

#include "common.h"
#include <stdbool.h>


const char* priority_to_str(Priority priority);
const char* incident_type_to_str(IncidentType type);
const char* unit_type_to_str(DepartmentType type);
const char* unit_state_to_str(UnitState state);
const char* incident_state_to_str(IncidentState state);




const char* getPriorityStr(Priority p);
const char* getIncidentTypeStr(IncidentType t);
const char* getUnitTypeStr(DepartmentType t);
const char* getUnitStateStr(UnitState s);
const char* getIncidentStateStr(IncidentState s);

int manhattan_distance(int x1, int y1, int x2, int y2);
bool is_valid_position(int x, int y);
void clear_input_buffer(void);


#endif // UTILS_H