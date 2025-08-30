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

# ~ Thirdparty ~
set(THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)

## boost
set(BOOST_DOWNLOAD_URL "https://github.com/boostorg/boost/releases/download/boost-1.89.0/boost-1.89.0-b2-nodocs.7z" CACHE STRING "boost release download URL")

## cxxopts
set(CXXOPTS_REPOSITORY "https://github.com/jarro2783/cxxopts.git" CACHE STRING "cxxopts repository URL")
set(CXXOPTS_VERSION "3.3.1" CACHE STRING "cxxopts version to fetch")

## fmt
set(FMT_REPOSITORY "https://github.com/fmtlib/fmt.git" CACHE STRING "fmt repository URL")
set(FMT_VERSION "11.2.0" CACHE STRING "fmt version to fetch")

## SFML
set(SFML_REPOSITORY "https://github.com/SFML/SFML.git" CACHE STRING "SFML repository URL")
set(SFML_VERSION "3.0.1" CACHE STRING "SFML version to fetch")
set(BUILD_SHARED_LIBS FALSE CACHE BOOL "Build SFML as static library" FORCE)

# ~ Compiler options ~
if(MSVC)
    add_compile_options(
        /MP           # Multi-processor Compilation
        /permissive-  # Strict standard conformance
    )

    add_compile_options(
        "$<$<CONFIG:Release>:/Ox>"     # Max. optimization
        "$<$<CONFIG:Release>:/Ob2>"    # Aggressive inlining
        "$<$<CONFIG:Release>:/Oi>"     # Intrinsic functions
        "$<$<CONFIG:Release>:/Ot>"     # Favor speed over size
        "$<$<CONFIG:Release>:/Oy>"     # Omit Frame Pointers
        "$<$<CONFIG:Release>:/GL>"     # Whole program optimization
    )
    add_link_options(
        "$<$<CONFIG:Release>:/LTCG>"  # Link-time code generation
    )

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR 
       CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(
        "$<$<CONFIG:Release>:-O3>"
        "$<$<CONFIG:Release>:-funroll-loops>"
        "$<$<CONFIG:Release>:-fomit-frame-pointer>"
        "$<$<CONFIG:Release>:-flto=auto>"            # Link-time optimization
    )
    add_link_options(
        "$<$<CONFIG:Release>:-flto=auto>"  # Link-time optimization
    )

else()
    message(WARNING "Unsupported CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
endif()

# ~ User options ~
## ENABLE_CHECKED_CASTS
option(ENABLE_CHECKED_CASTS "Enables cast runtime safety checks." ON)
## ENABLE_CONCURRENCY_CHECKS
option(ENABLE_CONCURRENCY_CHECKS "Enables concurrency runtime safety checks." ON)

## LOG_LEVEL
if(NOT DEFINED LOG_LEVEL)
    set(LOG_LEVEL "DEFAULT" CACHE STRING "Log level: DEFAULT | ERROR | WARN | SUCCESS | INFO | DEBUG | TRACE")
endif()

set_property(CACHE LOG_LEVEL PROPERTY STRINGS "DEFAULT" "ERROR" "WARN" "SUCCESS" "INFO" "DEBUG" "TRACE")

## EXTENDED_CHAR_SET
if(NOT DEFINED EXTENDED_CHAR_SET)
    if(WIN32)
        set(EXTENDED_CHAR_SET "CP437" CACHE STRING "Character set used for output: CP437 | ASCII")
    else()
        set(EXTENDED_CHAR_SET "ASCII" CACHE STRING "Character set used for output: CP437 | ASCII")
    endif()
endif()

set_property(CACHE EXTENDED_CHAR_SET PROPERTY STRINGS "CP437" "ASCII")

## COLOR_PRINT_MODE
if(NOT DEFINED COLOR_PRINT_MODE)
    set(COLOR_PRINT_MODE "ESCSEQ" CACHE STRING "Colored printing mode: ESCSEQ | WINAPI | DISABLED")
endif()
if(COLOR_PRINT_MODE STREQUAL "WINAPI" AND NOT WIN32)
    message(FATAL_ERROR "COLOR_PRINT_MODE = ${COLOR_PRINT_MODE} is only supported for Windows builds")
endif()

set_property(CACHE COLOR_PRINT_MODE PROPERTY STRINGS "ESCSEQ" "WINAPI" "DISABLED")

# ~ Info ~
message(STATUS "Settings:")
message(STATUS "Build:")
message(STATUS " > CMAKE_CXX_STANDARD = ${CMAKE_CXX_STANDARD}")
message(STATUS " > CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")
message(STATUS " > CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " > CMAKE_C_STANDARD = ${CMAKE_C_STANDARD}")
message(STATUS " > CMAKE_C_COMPILER = ${CMAKE_C_COMPILER}")
message(STATUS " > CMAKE_C_COMPILER_ID = ${CMAKE_C_COMPILER_ID}")
message(STATUS " > CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message(STATUS "Chemgine:")
message(STATUS " > ENABLE_CHECKED_CASTS = ${ENABLE_CHECKED_CASTS}")
message(STATUS " > ENABLE_CONCURRENCY_CHECKS = ${ENABLE_CONCURRENCY_CHECKS}")
message(STATUS " > LOG_LEVEL = ${LOG_LEVEL}")
message(STATUS " > EXTENDED_CHAR_SET = ${EXTENDED_CHAR_SET}")
message(STATUS " > COLOR_PRINT_MODE = ${COLOR_PRINT_MODE}")
message(STATUS "Thirdparty:")
message(STATUS " > BOOST_DOWNLOAD_URL = ${BOOST_DOWNLOAD_URL}")
message(STATUS " > CXXOPTS_REPOSITORY = ${CXXOPTS_REPOSITORY}")
message(STATUS " > CXXOPTS_VERSION = ${CXXOPTS_VERSION}")
message(STATUS " > FMT_REPOSITORY = ${FMT_REPOSITORY}")
message(STATUS " > FMT_VERSION = ${FMT_VERSION}")
message(STATUS " > SFML_REPOSITORY = ${SFML_REPOSITORY}")
message(STATUS " > SFML_VERSION = ${SFML_VERSION}")
message(STATUS " > BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}")

# ~ Projects ~
add_subdirectory(core)
add_subdirectory(gui_app)
add_subdirectory(defparse)
add_subdirectory(test_app)
