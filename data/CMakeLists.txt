_pkgconfig_invoke("dbus-1" DBUS SESSION_BUS_SERVICES_DIR "" "--variable=session_bus_services_dir")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/org.fcitx.HandWriting.service.in ${CMAKE_CURRENT_BINARY_DIR}/org.fcitx.HandWriting.service)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.fcitx.HandWriting.service DESTINATION ${DBUS_SESSION_BUS_SERVICES_DIR})