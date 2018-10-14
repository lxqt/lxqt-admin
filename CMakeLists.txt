project(lxqt-admin)
# CMP0000 - A minimum required CMake version must be specified.
# Note that the command invocation must appear in the CMakeLists.txt
# file itself; a call in an included file is not sufficient.
cmake_minimum_required(VERSION 3.6.2 FATAL_ERROR)


option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)

# Set default installation paths
set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}" CACHE PATH "Installation path for libraries")

find_package(Qt5Widgets REQUIRED)
find_package(lxqt REQUIRED)
find_package(KF5WindowSystem REQUIRED)
find_package(PolkitQt5-1 REQUIRED)

# Patch Version
set(LXQT_ADMIN_PATCH_VERSION 0)

set(LXQT_ADMIN_VERSION ${LXQT_MAJOR_VERSION}.${LXQT_MINOR_VERSION}.${LXQT_ADMIN_PATCH_VERSION})
add_definitions(
    "-DLXQT_ADMIN_VERSION=\"${LXQT_ADMIN_VERSION}\""
    "-DQT_NO_CAST_FROM_ASCII"
    "-DQT_NO_CAST_TO_ASCII"
    "-DQT_NO_URL_CAST_FROM_STRING"
    "-DQT_NO_CAST_FROM_BYTEARRAY"
)

include(LXQtPreventInSourceBuilds)
include(LXQtCompilerSettings NO_POLICY_SCOPE)
include(LXQtTranslate)

if(${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "NetBSD")
    message(WARNING "${CMAKE_SYSTEM_NAME} is not supported by lxqt-admin-user")
    message(WARNING "${CMAKE_SYSTEM_NAME} is not supported by lxqt-admin-time")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    add_subdirectory(lxqt-admin-user)
    message(WARNING "${CMAKE_SYSTEM_NAME} is not supported by lxqt-admin-time")
else()
    add_subdirectory(lxqt-admin-user)
    add_subdirectory(lxqt-admin-time)
endif()
