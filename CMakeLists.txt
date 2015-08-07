cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

project(lxqt-admin)

option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)

# Set default installation paths
set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}" CACHE PATH "Installation path for libraries")

find_package(Qt5Widgets REQUIRED QUIET)
find_package(lxqt REQUIRED QUIET)
find_package(KF5WindowSystem REQUIRED QUIET)

find_package(PkgConfig)
pkg_check_modules(OOBS REQUIRED
    glib-2.0
    liboobs-1
)

include(LXQtCompilerSettings NO_POLICY_SCOPE)
include(LXQtTranslate)

add_subdirectory(lxqt-admin-time)
add_subdirectory(lxqt-admin-user)

# building tarball with CPack -------------------------------------------------
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION_MAJOR ${LXQT_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${LXQT_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${LXQT_PATCH_VERSION})
set(CPACK_GENERATOR TBZ2)
set(CPACK_SOURCE_GENERATOR TBZ2)
set(CPACK_SOURCE_IGNORE_FILES /build/;.gitignore;.*~;.git;.kdev4;temp)
include(CPack)
