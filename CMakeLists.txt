cmake_minimum_required(VERSION 3.6)

project(umoria)

# Current version number
set(CURRENT_VERSION_MAJOR 5)
set(CURRENT_VERSION_MINOR 7)
set(CURRENT_VERSION_PATCH 1)

set(SOURCE_FILES_DIR ${PROJECT_SOURCE_DIR}/src)

# NOTE: currently disabled until Windows/MinGW support is added.
# configure a header file to pass some of the CMake settings to the source code
#configure_file(
#    "${SOURCE_FILES_DIR}/version.h.in"
#    "${PROJECT_BINARY_DIR}/version.h"
#)
## include directory for generated version.h
#include_directories("${PROJECT_BINARY_DIR}")

# add the binary tree to the search path for include files so that we will find header files
include_directories(${SOURCE_FILES_DIR})

set(SOURCE_FILES
    ${SOURCE_FILES_DIR}/main.cpp
    ${SOURCE_FILES_DIR}/misc1.cpp
    ${SOURCE_FILES_DIR}/misc2.cpp
    ${SOURCE_FILES_DIR}/misc3.cpp
    ${SOURCE_FILES_DIR}/misc4.cpp
    ${SOURCE_FILES_DIR}/store1.cpp
    ${SOURCE_FILES_DIR}/files.cpp
    ${SOURCE_FILES_DIR}/io.cpp
    ${SOURCE_FILES_DIR}/create.cpp
    ${SOURCE_FILES_DIR}/desc.cpp
    ${SOURCE_FILES_DIR}/generate.cpp
    ${SOURCE_FILES_DIR}/sets.cpp
    ${SOURCE_FILES_DIR}/dungeon.cpp
    ${SOURCE_FILES_DIR}/creature.cpp
    ${SOURCE_FILES_DIR}/death.cpp
    ${SOURCE_FILES_DIR}/eat.cpp
    ${SOURCE_FILES_DIR}/help.cpp
    ${SOURCE_FILES_DIR}/magic.cpp
    ${SOURCE_FILES_DIR}/potions.cpp
    ${SOURCE_FILES_DIR}/prayer.cpp
    ${SOURCE_FILES_DIR}/save.cpp
    ${SOURCE_FILES_DIR}/staffs.cpp
    ${SOURCE_FILES_DIR}/wands.cpp
    ${SOURCE_FILES_DIR}/scrolls.cpp
    ${SOURCE_FILES_DIR}/spells.cpp
    ${SOURCE_FILES_DIR}/wizard.cpp
    ${SOURCE_FILES_DIR}/store2.cpp
    ${SOURCE_FILES_DIR}/moria1.cpp
    ${SOURCE_FILES_DIR}/moria2.cpp
    ${SOURCE_FILES_DIR}/moria3.cpp
    ${SOURCE_FILES_DIR}/moria4.cpp
    ${SOURCE_FILES_DIR}/monsters.cpp
    ${SOURCE_FILES_DIR}/treasure.cpp
    ${SOURCE_FILES_DIR}/variable.cpp
    ${SOURCE_FILES_DIR}/rnd.cpp
    ${SOURCE_FILES_DIR}/recall.cpp
    ${SOURCE_FILES_DIR}/player.cpp
    ${SOURCE_FILES_DIR}/tables.cpp
)
add_executable(umoria ${SOURCE_FILES})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Werror -Wshadow -std=c++14")

# Link Curses
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
target_link_libraries(umoria ${CURSES_LIBRARIES})

# add the package install targets
install(TARGETS umoria
    RUNTIME
    DESTINATION ./
    COMPONENT applications
)
set(EXECUTABLE_OUTPUT_PATH ./umoria)

set(DATA_FILES
    ${PROJECT_SOURCE_DIR}/data/origcmds.hlp
    ${PROJECT_SOURCE_DIR}/data/owizcmds.hlp
    ${PROJECT_SOURCE_DIR}/data/roglcmds.hlp
    ${PROJECT_SOURCE_DIR}/data/rwizcmds.hlp
    ${PROJECT_SOURCE_DIR}/data/splash.hlp
    ${PROJECT_SOURCE_DIR}/data/version.hlp
    ${PROJECT_SOURCE_DIR}/data/welcome.hlp
)
file(COPY ${DATA_FILES} DESTINATION ./umoria/data)

set(SUPPORT_FILES
    ${PROJECT_SOURCE_DIR}/data/scores.dat
    ${PROJECT_SOURCE_DIR}/docs/manual.md
    ${PROJECT_SOURCE_DIR}/docs/faq.md
    ${PROJECT_SOURCE_DIR}/LICENSE
)
file(COPY ${SUPPORT_FILES} DESTINATION ./umoria)

# CPack configuration

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_VENDOR "umoria.org")

set(CPACK_PACKAGE_DESCRIPTION "Umoria")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Umoria: The Dungeons of Moria - A Classic Roguelike")
set(CPACK_PACKAGE_VERSION_MAJOR ${CURRENT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CURRENT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CURRENT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

set(CPACK_COMPONENTS_ALL application gameData supportData)

if(WIN32 AND NOT UNIX)
    # Windows MinGW not currently support, please use makefile.win.
else(WIN32 AND NOT UNIX)
    set(CPACK_GENERATOR "ZIP")

    if(APPLE)
        set(CPACK_SYSTEM_NAME "macos")
    endif(APPLE)
endif(WIN32 AND NOT UNIX)

include(CPack)
