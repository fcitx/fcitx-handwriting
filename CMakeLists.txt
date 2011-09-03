project(fcitx-handwriting)

cmake_minimum_required(VERSION 2.6)

find_package(PkgConfig REQUIRED)
find_package(X11 REQUIRED)
find_package(Fcitx REQUIRED)
pkg_check_modules(GTK2 "gtk+-2.0" REQUIRED)
pkg_check_modules(DBUS "dbus-1" REQUIRED)
pkg_check_modules(DBUS_GLIB "dbus-glib-1" REQUIRED)
pkg_check_modules(ZINNIA "zinnia" REQUIRED)

set(BINDIR ${CMAKE_INSTALL_PREFIX}/bin)
set(DATADIR ${CMAKE_INSTALL_PREFIX}/share)

configure_file(config.h.in config.h)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

set(CMAKE_C_FLAGS "-Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -fvisibility=hidden")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined,--as-needed")
set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--no-undefined,--as-needed")

add_subdirectory(src)
add_subdirectory(theme)
add_subdirectory(im)
add_subdirectory(data)
add_subdirectory(po)