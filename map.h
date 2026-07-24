#ifndef MAP_H
#define MAP_H

#include "common.h"
#include <stdbool.h>

typedef struct {
    char symbol[5];
} MapCell;

extern MapCell map[MAP_MAX_HEIGHT][MAP_MAX_WIDTH];
extern int map_width;
extern int map_height;


void map_init(int width, int height);
void map_clear_incident(int x, int y);
void clear_map(void);
void render_map(void);
void update_map(void);
void update_and_render(void);
void place_departments(void);
void place_incidents(void);
void place_units(void);
bool is_incident_cell(int x, int y);
bool is_department_cell(int x, int y);
int map_get_width(void);
int map_get_height(void);

#endif // MAP_H