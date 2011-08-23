
include_directories(
    ${GTK2_INCLUDE_DIRS}
    ${DBUS_GLIB_INCLUDE_DIRS}
    ${ZINNIA_INCLUDE_DIRS}
    ${X11_XTest_INCLUDE_PATH}
    ${PROJECT_BINARY_DIR}
)
link_directories(
    ${GTK2_LIBRARY_DIRS}
    ${DBUS_GLIB_LIBRARY_DIRS}
    ${ZINNIA_LIBRARY_DIRS}
)

set(FCITX_HANDWRITE_SRCS
    handwrite.c
    stroke.c
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
