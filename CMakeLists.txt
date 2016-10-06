cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

project(lxqt-admin)

option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)

# Set default installation paths
set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}" CACHE PATH "Installation path for libraries")

set(LXQTBT_MINIMUM_VERSION "0.1.0")

find_package(Qt5Widgets REQUIRED QUIET)
find_package(lxqt-build-tools ${LXQTBT_MINIMUM_VERSION} REQUIRED)
find_package(lxqt REQUIRED QUIET)
find_package(KF5WindowSystem REQUIRED QUIET)

include(LXQtCompilerSettings NO_POLICY_SCOPE)
include(LXQtTranslate)

add_subdirectory(lxqt-admin-time)
add_subdirectory(lxqt-admin-user)
