cmake_minimum_required(VERSION 3.10)
project(SEqlite)

set(CMAKE_CXX_STANDARD 14)

# Aggiunta di sorgenti della libreria
set(LIB_SOURCES
        sqlite3.c
)

# Aggiunta degli header della libreria
set(LIB_HEADERS
        sqlite3.h
)

# Aggiunta della libreria statica
add_library(SEqlite STATIC
        ${LIB_SOURCES}
        ${LIB_HEADERS}
)