cmake_minimum_required(VERSION 2.6)

project(lxqt-admin)

# additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Set default installation paths
set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}" CACHE PATH "Installation path for libraries")

# Support different versions of Qt
option(USE_QT5 "Force use the Qt5." $ENV{USE_QT5})
option(USE_QT4 "Force use the Qt4." $ENV{USE_QT4})
if((USE_QT4 AND USE_QT5) OR
   (NOT USE_QT4 AND NOT USE_QT5))
    # Autodetect Qt version
    find_package(Qt4 QUIET)
    if (QT4_FOUND)
        set(USE_QT4 ON)
        set(USE_QT5 OFF)
    else()
        set(USE_QT4 OFF)
        set(USE_QT5 ON)
    endif()
endif()

if(USE_QT5)
    find_package(Qt5Widgets REQUIRED QUIET)
    set(QTX_LIBRARIES ${Qt5Widgets_LIBRARIES})

    find_package(lxqt-qt5 REQUIRED QUIET)
    include(${LXQT_USE_FILE})
else()
	find_package(Qt4 REQUIRED)
	include(${QT_USE_FILE})
    set(QTX_LIBRARIES ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

	find_package(LXQT REQUIRED)
	include(${LXQT_USE_FILE})
endif()

find_package(PkgConfig)
pkg_check_modules(OOBS REQUIRED
  glib-2.0
  liboobs-1
)

add_subdirectory(lxqt-admin-time)
add_subdirectory(lxqt-admin-user)

# building tarball with CPack -------------------------------------------------
include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_VERSION_MAJOR ${LXQT_MAJOR_VERSION})
set (CPACK_PACKAGE_VERSION_MINOR ${LXQT_MINOR_VERSION})
set (CPACK_PACKAGE_VERSION_PATCH ${LXQT_PATCH_VERSION})
set (CPACK_GENERATOR TBZ2)
set (CPACK_SOURCE_GENERATOR TBZ2)
set (CPACK_SOURCE_IGNORE_FILES /build/;.gitignore;.*~;.git;.kdev4;temp)
include (CPack)
