project(fcitx-handwriting)

cmake_minimum_required(VERSION 2.6)

find_package(PkgConfig REQUIRED)
find_package(X11 REQUIRED)
pkg_check_modules(GTK2 "gtk+-2.0" REQUIRED)
pkg_check_modules(DBUS_GLIB "dbus-glib-1" REQUIRED)
pkg_check_modules(ZINNIA "zinnia" REQUIRED)

set(DATADIR ${CMAKE_INSTALL_PREFIX}/share)

configure_file(config.h.in config.h)

add_subdirectory(src)
add_subdirectory(theme)