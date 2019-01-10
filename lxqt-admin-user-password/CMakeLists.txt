project(lxqt-admin-user-password)

# build static helper class first
include_directories (
    ${CMAKE_CURRENT_BINARY_DIR}
)

# define sources
file(GLOB HEADERS *.h)
file(GLOB UI_FORMS *.ui)
file(GLOB SOURCES *.cpp)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(ALL_SOURCES
    ${SOURCES}
    ${UI_FORMS}
    ${HEADERS}
)

add_definitions(-DPROJECT_NAME="${PROJECT_NAME}")

# Translations **********************************
lxqt_translate_ts(muser_QM_FILES
    UPDATE_TRANSLATIONS YES
    UPDATE OPTIONS
    SOURCES
    ${MOCS}
    ${SOURCES}
    ${UI_FORMS}
    INSTALL_DIR
    "${LXQT_TRANSLATIONS_DIR}/${PROJECT_NAME}"
    PULL_TRANSLATIONS NO
)

lxqt_app_translation_loader(lxqt-admin-user-password_QM_LOADER ${PROJECT_NAME})
#************************************************

add_executable(lxqt-admin-user-password
    ${ALL_SOURCES}
    ${lxqt-admin-user-password_QM_FILES}
    ${lxqt-admin-user-password_QM_LOADER}
)

target_link_libraries(lxqt-admin-user-password
    KF5::WindowSystem
    Qt5::Widgets
    lxqt
    pam
)

install(TARGETS lxqt-admin-user-password RUNTIME DESTINATION bin)
