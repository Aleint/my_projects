cmake_minimum_required(VERSION 3.10)
project(cpp)

#
# To build Cryptomator library use the first option
# To build example.cpp executable use the second option
#
set(CMAKE_BUILD_CONFIGURATION_TYPE "Cryptomator")
# set(CMAKE_BUILD_CONFIGURATION_TYPE "Example")

if (CMAKE_BUILD_CONFIGURATION_TYPE STREQUAL "Cryptomator")

    # Specify the location of the JNI libraries
#    set(JAVA_HOME "$ENV{HOME}/.jdks/corretto-17.0.11")
    #set(JAVA_HOME "/usr/lib/jvm/java-17-openjdk-amd64")
    set(JAVA_HOME "$ENV{JAVA_HOME}")

    # Verify if the directory path exists
    if (EXISTS ${JAVA_HOME})
        message(STATUS "Found Java at: ${JAVA_HOME}")
    else ()
        message(FATAL_ERROR "Java not found at: ${JAVA_HOME}. Please specify the correct path.")
    endif ()

    # Check the platform and set JNI paths accordingly
    if (WIN32)
	set(JAVA_AWT_INCLUDE_PATH "${JAVA_HOME}/include")
        set(JAVA_INCLUDE_PATH2 "${JAVA_HOME}/include/win32")

    elseif (UNIX AND NOT APPLE)  # For Linux and other UNIX systems, but excluding macOS
        set(JAVA_AWT_INCLUDE_PATH "${JAVA_HOME}/include")
        set(JAVA_INCLUDE_PATH2 "${JAVA_HOME}/include/linux")

    elseif (APPLE)
        set(JAVA_AWT_INCLUDE_PATH "${JAVA_HOME}/include")
        set(JAVA_INCLUDE_PATH2 "${JAVA_HOME}/include/darwin")

    else ()
        message(FATAL_ERROR "Unsupported platform. Cannot set JNI paths.")
    endif ()

    # Include the necessary JNI directories
    include_directories(${JAVA_AWT_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})


    set(CMAKE_CXX_STANDARD 14)

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

    set(SECubeAPI_SOURCES
            SECubeAPI.cpp
            SECubeAPI.h
            configuration.h
    )

    set(SECubeUtils_SOURCES
            SECubeUtils.cpp
            SECubeUtils.h
    )

    set(SECubeConnector_SOURCES
            SECubeConnector.cpp
            ${SECubeAPI_SOURCES}
            ${SECubeUtils_SOURCES}
    )

    add_library(SECubeConnector SHARED ${SECubeConnector_SOURCES})

    add_subdirectory(sources/L0)
    add_subdirectory(sources/L1)
    add_subdirectory(sefile)
    add_subdirectory(sqlite)


    target_link_libraries(SECubeConnector L0 L1 SEfile SEqlite)

else ()

    set(CMAKE_CXX_STANDARD 14)

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

    set(SECubeAPI_SOURCES
            SECubeAPI.cpp
            SECubeAPI.h
            configuration.h
    )

    set(SECubeUtils_SOURCES
            SECubeUtils.cpp
            SECubeUtils.h
    )

    set(SECubeConfigurator_SOURCES
            SECubeConfiguration.cpp
            SECubeConfiguration.h
    )

    add_executable(SECubeExample
            example.cpp
            ${SECubeAPI_SOURCES}
            ${SECubeUtils_SOURCES}
            ${SECubeConfigurator_SOURCES}
    )

    add_subdirectory(sources/L0)
    add_subdirectory(sources/L1)
    add_subdirectory(sefile)
    add_subdirectory(sqlite)

    target_compile_definitions(SECubeExample PRIVATE DISABLE_JNI)
    target_link_libraries(SECubeExample L0 L1 SEfile SEqlite)

endif ()