cmake_minimum_required(VERSION 3.10)
project(L0)

set(CMAKE_CXX_STANDARD 14)

# Aggiunta dei file sorgente
set(LIB_SOURCES
        L0.cpp
        L0_Base/L0_base.cpp
        L0_commodities.cpp
        L0_communication.cpp
        L0_provision.cpp
)

# Aggiunta degli header
set(LIB_HEADERS
        L0.h
        L0_Base/L0_base.h
        L0_enumerations.h
        L0_error_manager.h
        Commodities_API/commodities_api.h
        Communication_API/communication_api.h
        Provision_API/provision_api.h
)

# Aggiunta dell'eseguibile
add_library(L0 STATIC ${LIB_SOURCES} ${LIB_HEADERS})

if (WIN32)
    target_link_libraries(L0 PRIVATE bcrypt)
endif ()