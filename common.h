#ifndef COMMON_H
#define COMMON_H

#include <time.h>
#include <stdbool.h>

// ////defines////
#define MAX_DEPARTMENTS 9
#define MAX_UNITS_PER_DEPARTMENT 9
#define MAX_INCIDENTS 100
#define MAP_MAX_WIDTH 100
#define MAP_MAX_HEIGHT 100
#define MAX_REQUIRED_UNITS_PER_TYPE 5
#define MAX_SYMBOL_LEN 8


#define MENU_INTERVAL 10

//log defines
#define LOG_FILENAME_PREFIX "log_"
#define LOG_FILENAME_EXT    ".txt"
#define LOG_FILENAME_MAX_LEN 50

//enums
typedef enum { FIRE, MEDICAL, POLICE } IncidentType;
typedef enum { LOW, MEDIUM, HIGH } Priority;
typedef enum {
    UNIT_WAITING,
    UNIT_DISPATCHED,
    UNIT_OPERATING,
    UNIT_RETURNING,
    UNIT_RESTING
} UnitState;
typedef enum {
    INCIDENT_WAITING,
    INCIDENT_OPERATION,
    INCIDENT_FINISHED
} IncidentState;



typedef enum {
    INCIDENT_FIRE,
    INCIDENT_MEDICAL,
    INCIDENT_CRIME,
    INCIDENT_TYPE_COUNT
} DetailedIncidentType;

typedef enum {
    PRIORITY_LOW,
    PRIORITY_MEDIUM,
    PRIORITY_HIGH,
    PRIORITY_COUNT
} DetailedPriority;

typedef enum {
    DEPT_FIRE,
    DEPT_MEDICAL,
    DEPT_POLICE,
    DEPT_TYPE_COUNT
} DepartmentType;



typedef struct Unit Unit;
typedef struct Department Department;
typedef struct Incident Incident;

//map
struct MapCell {
    char symbol[MAX_SYMBOL_LEN];
};

//color
#ifdef _WIN32
#include <windows.h>
#define ENABLE_COLORS() \
    do { \
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); \
        DWORD dwMode = 0; \
        GetConsoleMode(hOut, &dwMode); \
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; \
        SetConsoleMode(hOut, dwMode); \
    } while(0)
#else
#define ENABLE_COLORS()
#endif

#ifndef NO_COLORS
// Basic Colors
#define COLOR_RESET   "\x1b[0m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_PURPLE  "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_WHITE   "\x1b[37m"

// Map Colors
#define MAP_EMPTY     "\x1b[90m"
#define MAP_FIRE_DEPT "\x1b[38;5;208m"
#define MAP_MED_DEPT  "\x1b[38;5;34m"
#define MAP_POLICE_DEPT "\x1b[38;5;27m"
#define MAP_FIRE_UNIT "\x1b[38;5;214m"
#define MAP_MED_UNIT  "\x1b[38;5;82m"
#define MAP_POLICE_UNIT "\x1b[38;5;39m"

#define CONSOLE_DARK_PURPLE_BG "\x1b[48;5;53m"
#define CONSOLE_RESET_BG "\x1b[49m"
#define BRIGHT_YELLOW "\x1b[1;33m"
#define MAP_INCIDENT  "\x1b[1;31m"

#else
// Colorless Mode
#define COLOR_RESET   ""
#define COLOR_RED     ""
#define COLOR_GREEN   ""
#define COLOR_YELLOW  ""
#define COLOR_BLUE    ""
#define COLOR_PURPLE  ""
#define COLOR_CYAN    ""
#define COLOR_WHITE   ""
#define MAP_EMPTY     ""
#define MAP_FIRE_DEPT ""
#define MAP_MED_DEPT  ""
#define MAP_POLICE_DEPT ""
#define MAP_FIRE_UNIT ""
#define MAP_MED_UNIT  ""
#define MAP_POLICE_UNIT ""
#define CONSOLE_DARK_PURPLE_BG ""
#define CONSOLE_RESET_BG ""
#define BRIGHT_YELLOW ""
#define MAP_INCIDENT  ""
#endif

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0])

#endif // COMMON_H

