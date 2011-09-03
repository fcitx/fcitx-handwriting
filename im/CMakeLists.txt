include_directories(${FCITX4_FCITX_INCLUDE_DIRS})
include_directories(${FCITX4_FCITX_CONFIG_INCLUDE_DIRS})
include_directories(${FCITX4_FCITX_UTILS_INCLUDE_DIRS})
include_directories(${DBUS_INCLUDE_DIRS})

link_directories(${FCITX4_FCITX_LIBRARY_DIRS})
link_directories(${FCITX4_FCITX_CONFIG_LIBRARY_DIRS})
link_directories(${FCITX4_FCITX_UTILS_LIBRARY_DIRS})
link_directories(${DBUS_LIBRARY_DIRS})

set(HANDWRITING_IM_SRCS
    handwriting.c
)

fcitx_add_addon(fcitx-handwriting-im ${HANDWRITING_IM_SRCS})
set_target_properties(fcitx-handwriting-im PROPERTIES
    OUTPUT_NAME fcitx-handwriting
)
target_link_libraries(fcitx-handwriting-im
                      ${DBUS_LIBRARIES}
                      ${FCITX4_FCITX_LIBRARIES}
                      ${FCITX4_FCITX_CONFIG_LIBRARIES}
                      ${FCITX4_FCITX_UTILS_LIBRARIES}
)
fcitx_add_addon_conf_file(fcitx-handwriting.conf)
