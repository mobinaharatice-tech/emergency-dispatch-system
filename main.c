#include "log.h"
#include "config.h"
#include "map.h"
#include "incident.h"
#include "menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "stats.h"

void create_default_config(const char* path) {
    FILE* file = fopen(path, "w");
    if (file) {
        fprintf(file, "MAP_SIZE 20 15\n");
        fprintf(file, "DEPARTMENT FIRE 1 2 8 3\n");
        fprintf(file, "DEPARTMENT MEDICAL 2 5 5 5\n");
        fprintf(file, "DEPARTMENT POLICE 3 8 1 3\n");
        fclose(file);
        char log_msg[100];
        snprintf(log_msg, sizeof(log_msg), "Default config created at %s", path);
        log_event(log_msg);
    } else {
        log_error("Failed to create default config");
    }
}

bool try_load_config() {
    const char* config_paths[] = {"config.txt", "../config.txt"};
    for (int i = 0; i < 2; i++) {
        if (access(config_paths[i], F_OK) == 0) {
            if (config_load(config_paths[i])) {
                return true;
            }
        }
    }
    create_default_config("config.txt");
    return config_load("config.txt");
}

void initialize_system() {
    log_init();
    log_simulation_start();

    if (!try_load_config()) {
        log_error("Fatal: Could not load config");
        exit(EXIT_FAILURE);
    }

    map_init(map_get_width(), map_get_height());
    init_incidents();
    log_event("System initialized");
}

int main() {
    log_init();
    initialize_system();
    stats_init();
    ENABLE_COLORS();
    menu_init();
    menu_main_loop();
    log_close();
    return EXIT_SUCCESS;
}
