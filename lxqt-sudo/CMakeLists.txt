project(lxqt-sudo)

include_directories (
    ${CMAKE_CURRENT_BINARY_DIR}
    ${LXQT_INCLUDE_DIRS}
    ${QTXDG_INCLUDE_DIRS}
)

add_definitions("-DLXQTSUDO_INSTALL_DIR=\"${CMAKE_INSTALL_PREFIX}/bin\""
    "-DLXQTSUDO_SUDO=\"sudo\""
    "-DLXQTSUDO=\"lxqt-sudo\""
    "-DLXQTSUDO_HELPER=\"lxqt-sudo-helper\""
    )

set ( HDRS
    communication.h
    passworddialog.h
)

set ( SRCS
    communication.cpp
    passworddialog.cpp
    main.cpp
)

set ( MOCS
    communication.h
    passworddialog.h
)

set( UIS
    passworddialog.ui
)

qt5_wrap_cpp(MOC_OBJECTS ${MOCS})
qt5_wrap_ui(UI_HEADERS ${UIS})

# Translations **********************************
lxqt_translate_ts(QM_FILES
    UPDATE_TRANSLATIONS ${UPDATE_TRANSLATIONS}
    SOURCES
    ${HDRS}
    ${SRCS}
    ${UIS}
    INSTALL_DIR
    "${LXQT_TRANSLATIONS_DIR}/${PROJECT_NAME}"
)

lxqt_translate_desktop(DESKTOP_FILES
    SOURCES
    lxqt-sudo.desktop.in
)

lxqt_app_translation_loader(SRCS ${PROJECT_NAME})
#************************************************

add_executable(lxqt-sudo
    ${SRCS}
    ${UI_HEADERS}
    ${MOC_OBJECTS}
    ${DESKTOP_FILES}
    ${QM_FILES}
)
add_executable(lxqt-sudo-helper IMPORTED)

target_link_libraries(lxqt-sudo
    Qt5::Widgets
    ${LXQT_LIBRARIES}
)

add_custom_command(TARGET lxqt-sudo POST_BUILD
    COMMAND ln -f -s lxqt-sudo lxqt-sudo-helper
    WORKING_DIRECTORY .
    COMMENT "Creating make-sudo-helper symlink")

install(TARGETS lxqt-sudo RUNTIME DESTINATION bin)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/lxqt-sudo-helper" DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
install(FILES ${DESKTOP_FILES} DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications")
