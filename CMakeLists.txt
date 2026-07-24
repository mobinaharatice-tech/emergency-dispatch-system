cmake_minimum_required(VERSION 3.30)
project(untitled49 C)

set(CMAKE_C_STANDARD 11)

add_executable(untitled49 main.c
        menu.c
        map.c
        map.h
        menu.h
        common.h
        stats.c
        stats.h
        config.c
        config.h
        incident.c
        incident.h
        simulation.c
        simulation.h
        log.c
        log.h
        unit.c
        unit.h
        utils.c
        utils.h
        types.h
        unit_types.h)
