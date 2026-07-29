cmake_minimum_required(VERSION 3.16)
project(yhbos-calendar VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(KF6Holidays REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LUNAR_DATE REQUIRED lunar-date-3.0 glib-2.0 gobject-2.0)

qt_add_executable(yhbos-calendar
    main.cpp
    mainwindow.h mainwindow.cpp
    calendarwidget.h calendarwidget.cpp
    lunarcalendar.h lunarcalendar.cpp
    translations.qrc
)

target_link_libraries(yhbos-calendar PRIVATE
    Qt6::Widgets
    KF6::Holidays
    ${LUNAR_DATE_LIBRARIES}
)

target_include_directories(yhbos-calendar PRIVATE
    ${LUNAR_DATE_INCLUDE_DIRS}
)

target_compile_options(yhbos-calendar PRIVATE
    ${LUNAR_DATE_CFLAGS_OTHER}
)

install(TARGETS yhbos-calendar RUNTIME DESTINATION bin)

set(CPACK_PACKAGE_NAME "yhbos-calendar")
set(CPACK_PACKAGE_VERSION "1.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Desktop calendar with Lunar calendar support")
set(CPACK_PACKAGE_DESCRIPTION "A Qt6-based desktop calendar application with dual Gregorian/Lunar display, 24 solar terms, traditional Chinese festivals, and multi-language support.")
set(CPACK_PACKAGE_CONTACT "yhbos")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "yhbos")
set(CPACK_PACKAGE_SECTION "utils")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/yhbos/yhbos-calendar")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "qt6-base-dev (>= 6.0), liblunar-date-3.0-1 (>= 3.0), libkf6holidays6 (>= 6.0)")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_FILE_NAME "yhbos-calendar-1.0.deb")

include(CPack)
