project(lxqt-admin-time)

# build static helper class first
include_directories (
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTCORE_INCLUDE_DIR}
    ${QT_QTGUI_INCLUDE_DIR}
    ${LXQT_INCLUDE_DIRS}
    ${QTXDG_INCLUDE_DIRS}
    ${OOBS_INCLUDE_DIRS}
)

set ( lxqt-admin-time_HDRS
    timeadmindialog.h
)

set ( lxqt-admin-time_SRCS
    main.cpp
    timeadmindialog.cpp
)

set ( lxqt-admin-time_MOCS
    timeadmindialog.h
)

set( lxqt-admin-time_UIS
    timeadmindialog.ui
)

qt5_wrap_cpp(MOCS ${lxqt-admin-time_MOCS})
qt5_wrap_ui(UIS ${lxqt-admin-time_UIS})

# Translations **********************************
include(LxQtTranslate)
lxqt_translate_ts(lxqt-admin-time_QM_FILES
    SOURCES
    ${lxqt-admin-time_HDRS}
    ${lxqt-admin-time_SRCS}
    ${lxqt-admin-time_UIS}
    INSTALLATION_DIR
    ${CMAKE_INSTALL_PREFIX}/share/lxqt/lxqt-admin
)

lxqt_translate_desktop(DESKTOP_FILES
    SOURCES
    lxqt-admin-time.desktop.in
)
#************************************************

add_executable(lxqt-admin-time ${lxqt-admin-time_SRCS} ${UIS} ${MOCS} ${DESKTOP_FILES} ${lxqt-admin-time_QM_FILES})

target_link_libraries(lxqt-admin-time
    ${QTX_LIBRARIES}
    ${LXQT_LIBRARIES}
    ${OOBS_LIBRARIES}
)

install(TARGETS lxqt-admin-time RUNTIME DESTINATION bin)
install(FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
