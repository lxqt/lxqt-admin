cmake_minimum_required(VERSION 3.18.0 FATAL_ERROR)
# CMP0000: Call the cmake_minimum_required() command at the beginning of the top-level
# CMakeLists.txt file even before calling the project() command.
# The cmake_minimum_required(VERSION) command implicitly invokes the cmake_policy(VERSION)
# command to specify that the current project code is written for the given range of CMake
# versions.
project(lxqt-admin)

option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)

# Set default installation paths
set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}" CACHE PATH "Installation path for libraries")

# Minimum Versions
set(LXQT_MINIMUM_VERSION "2.2.0")
set(QT_MINIMUM_VERSION "6.6.0")

find_package(Qt6Widgets ${QT_MINIMUM_VERSION} REQUIRED)
find_package(lxqt ${LXQT_MINIMUM_VERSION} REQUIRED)
find_package(PolkitQt6-1 REQUIRED)

# Patch Version
set(LXQT_ADMIN_PATCH_VERSION 0)

set(LXQT_ADMIN_VERSION ${LXQT_MAJOR_VERSION}.${LXQT_MINOR_VERSION}.${LXQT_ADMIN_PATCH_VERSION})
add_definitions(
    "-DLXQT_ADMIN_VERSION=\"${LXQT_ADMIN_VERSION}\""
)

include(LXQtPreventInSourceBuilds)
include(LXQtCompilerSettings NO_POLICY_SCOPE)
include(LXQtTranslate)

if(${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "NetBSD")
    add_subdirectory(lxqt-admin-user)
    message(WARNING "${CMAKE_SYSTEM_NAME} is not supported by lxqt-admin-time")
else()
    add_subdirectory(lxqt-admin-user)
    add_subdirectory(lxqt-admin-time)
endif()
