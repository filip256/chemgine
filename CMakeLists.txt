list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(utils)
include(FetchContent)

cmake_minimum_required(VERSION 3.20)
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

# SFML
set(SFML_REPOSITORY "https://github.com/SFML/SFML.git" CACHE STRING "SFML repository URL")
set(SFML_VERSION "3.0.1" CACHE STRING "SFML version/tag to fetch")
set(BUILD_SHARED_LIBS FALSE CACHE BOOL "Build SFML as static library" FORCE)

# Compiler Options
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

# User Options
option(DISABLE_CHECKED_CASTS "Disables cast runtime safety checks." OFF)
option(DISABLE_CONCURRENCY_CHECKS "Disables concurrency runtime safety checks." OFF)

if(DISABLE_CHECKED_CASTS)
    target_compile_definitions(core CHG_DISABLE_CHECKED_CASTS)
endif()

if(DISABLE_CONCURRENCY_CHECKS)
    target_compile_definitions(core PUBLIC CHG_DISABLE_CONCURRENCY_CHECKS)
endif()

# Info
message(STATUS "Settings:")
message(STATUS "  Build:")
message(STATUS "    CMAKE_CXX_STANDARD = ${CMAKE_CXX_STANDARD}")
message(STATUS "    CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")
message(STATUS "    CMAKE_C_STANDARD = ${CMAKE_C_STANDARD}")
message(STATUS "    CMAKE_C_COMPILER = ${CMAKE_C_COMPILER}")
message(STATUS "  Chemgine:")
message(STATUS "    DISABLE_CHECKED_CASTS = ${DISABLE_CHECKED_CASTS}")
message(STATUS "    DISABLE_CONCURRENCY_CHECKS = ${DISABLE_CONCURRENCY_CHECKS}")
message(STATUS "  Thirdparty:")
message(STATUS "    boost:")
message(STATUS "      BOOST_INCLUDE_DIR = ${BOOST_INCLUDE_DIR}")
message(STATUS "    cxxopts:")
message(STATUS "      CXXOPTS_INCLUDE_DIR = ${CXXOPTS_INCLUDE_DIR}")
message(STATUS "    SFML:")
message(STATUS "      SFML_REPOSITORY = ${SFML_REPOSITORY}")
message(STATUS "      SFML_VERSION = ${SFML_VERSION}")
message(STATUS "      BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}")

# Chemgine
add_subdirectory(core)
add_subdirectory(gui_app)
add_subdirectory(defparse)
add_subdirectory(test_app)
# add_subdirectory(dev_tools)
