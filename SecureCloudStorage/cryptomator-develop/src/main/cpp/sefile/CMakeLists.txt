cmake_minimum_required(VERSION 3.10)
project(SEfile)

set(CMAKE_CXX_STANDARD 14)

# Aggiunta di sorgenti della libreria
set(LIB_SOURCES
        SEcureDB.cpp
        SEfile.cpp
)

# Aggiunta degli header della libreria
set(LIB_HEADERS
        environment.h
        SEcureDB.h
        SEfile.h
        SEfile_C_interface.h
)

# Aggiunta della libreria statica
add_library(SEfile
        STATIC
        ${LIB_SOURCES}
        ${LIB_HEADERS}
)