project(lxqt-admin-time)

# build static helper class first
include_directories (
    ${CMAKE_CURRENT_BINARY_DIR}
)

set ( lxqt-admin-time_HDRS
    timeadmindialog.h
    datetime.h
    timezone.h
    itimedatectl.h
    dbustimedatectl.h
    fbsdtimedatectl.h
    nosdtimedatectl.h
)

set ( lxqt-admin-time_SRCS
    main.cpp
    timeadmindialog.cpp
    datetime.cpp
    timezone.cpp
    fbsdtimedatectl.cpp
    dbustimedatectl.cpp
    nosdtimedatectl.cpp
)

set ( lxqt-admin-time_MOCS
    timeadmindialog.h
    datetime.h
    timezone.h
)

set( lxqt-admin-time_UIS
    datetime.ui
    timezone.ui
)

qt6_wrap_cpp(MOCS ${lxqt-admin-time_MOCS})
qt6_wrap_ui(UIS ${lxqt-admin-time_UIS})

# Translations **********************************
lxqt_translate_ts(lxqt-admin-time_QM_FILES
    UPDATE_TRANSLATIONS ${UPDATE_TRANSLATIONS}
    SOURCES
    ${lxqt-admin-time_HDRS}
    ${lxqt-admin-time_SRCS}
    ${lxqt-admin-time_UIS}
    INSTALL_DIR
    "${LXQT_TRANSLATIONS_DIR}/${PROJECT_NAME}"
)

lxqt_translate_desktop(DESKTOP_FILES
    SOURCES
    lxqt-admin-time.desktop.in
    USE_YAML
)

lxqt_app_translation_loader(lxqt-admin-time_QM_LOADER ${PROJECT_NAME})
#************************************************

find_package(PkgConfig QUIET)
pkg_check_modules(LIBSYSTEMD QUIET libsystemd)
if(NOT LIBSYSTEMD_FOUND)
    add_definitions(-DNO_SYSTEMD)
endif()

add_executable(lxqt-admin-time
    ${lxqt-admin-time_SRCS}
    ${UIS}
    ${MOCS}
    ${DESKTOP_FILES}
    ${lxqt-admin-time_QM_FILES}
    ${lxqt-admin-time_QM_LOADER}
)

target_link_libraries(lxqt-admin-time
    Qt6::Widgets
    Qt6::DBus
    lxqt
)
install(TARGETS lxqt-admin-time RUNTIME DESTINATION bin)
install(FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
install(PROGRAMS "lxqt-admin-time-helper.freebsd" DESTINATION bin RENAME lxqt-admin-time-helper)
install(FILES "org.lxqt.lxqt-admin-time.policy" DESTINATION "${POLKITQT-1_POLICY_FILES_INSTALL_DIR}")
else()
if(NOT LIBSYSTEMD_FOUND)
configure_file(org.lxqt.lxqt-admin-time.policy.nosd.in "${CMAKE_CURRENT_BINARY_DIR}/org.lxqt.lxqt-admin-time.policy.nosd")
install (PROGRAMS "lxqt-admin-time-helper.nosystemd" DESTINATION bin RENAME lxqt-admin-time-helper)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/org.lxqt.lxqt-admin-time.policy.nosd" DESTINATION "${POLKITQT-1_POLICY_FILES_INSTALL_DIR}" RENAME org.lxqt.lxqt-admin-time.policy)
endif()
endif()
