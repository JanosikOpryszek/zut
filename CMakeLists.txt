cmake_minimum_required (VERSION 2.6)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

project (pr)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++98 -Lpthread ")


# Must use GNUInstallDirs to install libraries into correct locations on all platforms:
include(GNUInstallDirs)

# Pkg_config is used to locate header and files for dependency libraries:
find_package(PkgConfig)

# Defines variables GTKMM_INCLUDE_DIRS, GTKMM_LIBRARY_DIRS and GTKMM_LIBRARIES.
pkg_check_modules(GTKMM gtkmm-3.0)
link_directories( ${GTKMM_LIBRARY_DIRS} )
include_directories( ${GTKMM_INCLUDE_DIRS} )



find_package (Threads)
add_executable (pr 

    ILogger.h
    Loger.hpp
    Loger.cpp
    main.cpp
    Candriverserver.cpp
    Candriverserver.hpp
    eErrorCodes.h
    ICandriverserver.cpp
    ICandriverserver.hpp
    imsgvermethod.cpp
    imsgvermethod.hpp

    )
target_link_libraries (pr ${CMAKE_THREAD_LIBS_INIT} ${GTKMM_LIBRARIES})
