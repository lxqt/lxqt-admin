project(lxqt-admin-user)

# build static helper class first
include_directories (
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTCORE_INCLUDE_DIR}
    ${QT_QTGUI_INCLUDE_DIR}
    ${LXQT_INCLUDE_DIRS}
    ${QTXDG_INCLUDE_DIRS}
    ${OOBS_INCLUDE_DIRS}
)

set ( lxqt-admin-user_HDRS
    maindialog.h
)

set ( lxqt-admin-user_SRCS
    main.cpp
    maindialog.cpp
)

set ( lxqt-admin-user_MOCS
    maindialog.h
)

set( lxqt-admin-user_UIS
    maindialog.ui
)

qt4_wrap_cpp(MOCS ${lxqt-admin-user_MOCS})
qt4_wrap_ui(UIS ${lxqt-admin-user_UIS})

# Translations **********************************
include(LxQtTranslate)
lxqt_translate_ts(lxqt-admin-user_QM_FILES
    SOURCES
    ${lxqt-admin-user_HDRS}
    ${lxqt-admin-user_SRCS}
    ${lxqt-admin-user_UIS}
    INSTALLATION_DIR
    ${CMAKE_INSTALL_PREFIX}/share/lxqt/lxqt-admin
)

lxqt_translate_desktop(DESKTOP_FILES
    SOURCES
    lxqt-admin-user.desktop.in
)
#************************************************

add_executable(lxqt-admin-user ${lxqt-admin-user_SRCS} ${UIS} ${MOCS} ${DESKTOP_FILES} ${lxqt-admin-user_QM_FILES})

target_link_libraries(lxqt-admin-user
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QTXDG_LIBRARIES}
    ${LXQT_LIBRARIES}
    ${OOBS_LIBRARIES}
)

install(TARGETS lxqt-admin-user RUNTIME DESTINATION bin)
install(FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
