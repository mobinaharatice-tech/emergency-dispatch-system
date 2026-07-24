#ifndef CONFIG_H
#define CONFIG_H
#include "unit.h"
#include "common.h"


void load_configuration(const char *filename);
IncidentType parse_department_type(const char *str);
Priority parse_priority(const char *str);


bool config_load(const char* filename);
void config_cleanup(void);


int config_get_map_width(void);
int config_get_map_height(void);
const Department* config_get_departments(void);
int config_get_department_count(void);


extern Department departments[MAX_DEPARTMENTS];
extern int department_count;
extern int map_width;
extern int map_height;

#endif // CONFIG_H