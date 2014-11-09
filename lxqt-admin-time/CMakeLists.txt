project(lxqt-admin-time)

# build static helper class first
include_directories (
    ${CMAKE_CURRENT_BINARY_DIR}
    ${LXQT_INCLUDE_DIRS}
    ${QTXDG_INCLUDE_DIRS}
    ${OOBS_INCLUDE_DIRS}
)

set ( lxqt-admin-time_HDRS
    timeadmindialog.h
    datetime.h
    timezone.h
)

set ( lxqt-admin-time_SRCS
    main.cpp
    timeadmindialog.cpp
    datetime.cpp
    timezone.cpp
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

qt5_wrap_cpp(MOCS ${lxqt-admin-time_MOCS})
qt5_wrap_ui(UIS ${lxqt-admin-time_UIS})

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
)

lxqt_app_translation_loader(lxqt-admin-time_QM_LOADER ${PROJECT_NAME})
#************************************************

add_executable(lxqt-admin-time
    ${lxqt-admin-time_SRCS}
    ${UIS}
    ${MOCS}
    ${DESKTOP_FILES}
    ${lxqt-admin-time_QM_FILES}
    ${lxqt-admin-time_QM_LOADER}
)

target_link_libraries(lxqt-admin-time
    KF5::WindowSystem
    Qt5::Widgets
    ${LXQT_LIBRARIES}
    ${OOBS_LIBRARIES}
)

install(TARGETS lxqt-admin-time RUNTIME DESTINATION bin)
install(FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
