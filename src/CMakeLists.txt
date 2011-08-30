
include_directories(
    ${GTK2_INCLUDE_DIRS}
    ${DBUS_GLIB_INCLUDE_DIRS}
    ${ZINNIA_INCLUDE_DIRS}
    ${X11_XTest_INCLUDE_PATH}
    ${PROJECT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)
link_directories(
    ${GTK2_LIBRARY_DIRS}
    ${DBUS_GLIB_LIBRARY_DIRS}
    ${ZINNIA_LIBRARY_DIRS}
)

find_program(DBUSBINDINGTOOL dbus-binding-tool)

set(FCITX_HANDWRITE_SRCS
    service.c
    handwrite.c
    stroke.c
    handwriting-service.h
)

add_custom_command(
    OUTPUT handwriting-service.h
    COMMAND ${DBUSBINDINGTOOL} --mode=glib-server
                               --output=handwriting-service.h
                               --prefix=handwriting_service
                                ${CMAKE_CURRENT_SOURCE_DIR}/handwriting-service.xml
    DEPENDS handwriting-service.xml
)

add_executable(fcitx-handwriting ${FCITX_HANDWRITE_SRCS})
target_link_libraries(fcitx-handwriting
    ${GTK2_LIBRARIES}
    ${DBUS_GLIB_LIBRARIES}
    ${ZINNIA_LIBRARIES}
    ${X11_XTest_LIB}
    ${X11_LIBRARIES}
)
install(TARGETS fcitx-handwriting DESTINATION bin)
