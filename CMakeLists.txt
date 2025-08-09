list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(utils)

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

# SFML
set(SFML_ROOT ${THIRDPARTY_DIR}/SFML)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SFML_INCLUDE_DIR ${SFML_ROOT}/x64/include)
    set(SFML_LIB_DIR ${SFML_ROOT}/x64/lib)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(SFML_INCLUDE_DIR ${SFML_ROOT}/x86/include)
    set(SFML_LIB_DIR ${SFML_ROOT}/x86/lib)
else()
    message(FATAL_ERROR "Unknown architecture")
endif()

set(SFML_INCLUDE_DIR ${SFML_INCLUDE_DIR} CACHE PATH "SFML include path")
set(SFML_LIB_DIR ${SFML_LIB_DIR} CACHE PATH "SFML lib path")

# cxxopts
set(CXXOPTS_ROOT ${THIRDPARTY_DIR}/cxxopts)
set(CXXOPTS_INCLUDE_DIR ${CXXOPTS_ROOT}/include CACHE PATH "cxxopts include path")

# Compiler Options
if(MSVC)
    add_compile_options(
        /WX          # Treat warnings as errors
        /W3          # Warning Level TODO: Move to W4
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

message(STATUS "DISABLE_CHECKED_CASTS = ${DISABLE_CHECKED_CASTS}")
message(STATUS "DISABLE_CONCURRENCY_CHECKS = ${DISABLE_CONCURRENCY_CHECKS}")

if(DISABLE_CHECKED_CASTS)
    target_compile_definitions(core CHG_DISABLE_CHECKED_CASTS)
endif()

if(DISABLE_CONCURRENCY_CHECKS)
    target_compile_definitions(core PUBLIC CHG_DISABLE_CONCURRENCY_CHECKS)
endif()

# Chemgine
add_subdirectory(core)
add_subdirectory(gui_app)
add_subdirectory(defparse)
add_subdirectory(test_app)
# add_subdirectory(dev_tools)
