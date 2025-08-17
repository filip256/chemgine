list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(utils)
include(FetchContent)
cmake_minimum_required(VERSION 3.20)

set(VERSION "0.0.1")

message(STATUS "Configuring Chemgine v${VERSION}")
project(Chemgine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Static linking
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Thirdparty
set(THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)

# boost
set(BOOST_ROOT ${THIRDPARTY_DIR}/boost)
set(BOOST_INCLUDE_DIR ${BOOST_ROOT}/include CACHE PATH "boost include path")

# cxxopts
set(CXXOPTS_ROOT ${THIRDPARTY_DIR}/cxxopts)
set(CXXOPTS_INCLUDE_DIR ${CXXOPTS_ROOT}/include CACHE PATH "cxxopts include path")

# fmt
set(FMT_REPOSITORY "https://github.com/fmtlib/fmt.git" CACHE STRING "fmt repository URL")
set(FMT_VERSION "11.2.0" CACHE STRING "fmt version to fetch")

# SFML
set(SFML_REPOSITORY "https://github.com/SFML/SFML.git" CACHE STRING "SFML repository URL")
set(SFML_VERSION "3.0.1" CACHE STRING "SFML version to fetch")
set(BUILD_SHARED_LIBS FALSE CACHE BOOL "Build SFML as static library" FORCE)

# Compiler options
if(MSVC)
    add_compile_options(
        /MP          # Multi-processor Compilation
        /permissive- # Strict standard conformance
    )

    add_compile_options("$<$<CONFIG:Release>:/O2>")
    add_compile_options("$<$<CONFIG:Release>:/Ob2>")
    add_compile_options("$<$<CONFIG:Release>:/Oi>")
    add_compile_options("$<$<CONFIG:Release>:/Ot>")
    add_compile_options("$<$<CONFIG:Release>:/Oy>")
    add_compile_options("$<$<CONFIG:Release>:/GT>")
    add_compile_options("$<$<CONFIG:Release>:/GL>")
    
    add_link_options("$<$<CONFIG:Release>:/LTCG>")
endif()

# User options
option(DISABLE_CHECKED_CASTS "Disables cast runtime safety checks." OFF)
option(DISABLE_CONCURRENCY_CHECKS "Disables concurrency runtime safety checks." OFF)
option(DISABLE_COLORED_PRINTING "Disables colored stdout/stderr printing." OFF)

if(NOT DEFINED EXTENDED_CHAR_SET)
    if(WIN32)
        set(EXTENDED_CHAR_SET "CP437" CACHE STRING "Character set used for output: CP437 | ASCII")
    else()
        set(EXTENDED_CHAR_SET "ASCII" CACHE STRING "Character set used for output: CP437 | ASCII")
    endif()
endif()

# Info
message(STATUS "Settings:")
message(STATUS "  Build:")
message(STATUS "   > CMAKE_CXX_STANDARD = ${CMAKE_CXX_STANDARD}")
message(STATUS "   > CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")
message(STATUS "   > CMAKE_C_STANDARD = ${CMAKE_C_STANDARD}")
message(STATUS "   > CMAKE_C_COMPILER = ${CMAKE_C_COMPILER}")
message(STATUS "   > CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message(STATUS "  Chemgine:")
message(STATUS "   > DISABLE_CHECKED_CASTS = ${DISABLE_CHECKED_CASTS}")
message(STATUS "   > DISABLE_CONCURRENCY_CHECKS = ${DISABLE_CONCURRENCY_CHECKS}")
message(STATUS "   > DISABLE_COLORED_PRINTING = ${DISABLE_COLORED_PRINTING}")
message(STATUS "   > EXTENDED_CHAR_SET = ${EXTENDED_CHAR_SET}")
message(STATUS "  Thirdparty:")
message(STATUS "    boost:")
message(STATUS "     > BOOST_INCLUDE_DIR = ${BOOST_INCLUDE_DIR}")
message(STATUS "    cxxopts:")
message(STATUS "     > CXXOPTS_INCLUDE_DIR = ${CXXOPTS_INCLUDE_DIR}")
message(STATUS "    fmt:")
message(STATUS "     > FMT_REPOSITORY = ${FMT_REPOSITORY}")
message(STATUS "     > FMT_VERSION = ${FMT_VERSION}")
message(STATUS "    SFML:")
message(STATUS "     > SFML_REPOSITORY = ${SFML_REPOSITORY}")
message(STATUS "     > SFML_VERSION = ${SFML_VERSION}")
message(STATUS "     > BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}")

# Chemgine
add_subdirectory(core)
add_subdirectory(gui_app)
add_subdirectory(defparse)
add_subdirectory(test_app)
# add_subdirectory(dev_tools)
