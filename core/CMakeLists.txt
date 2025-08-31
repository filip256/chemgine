file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

add_library(core STATIC ${SOURCES})

# Generate VS filters based on dir structure.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ~ Compiler options ~
if(MSVC)
    target_compile_options(core PUBLIC
        /W4     # Warning Level
        /WX     # Treat warnings as errors
        /utf-8
    )

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
       CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(core PUBLIC
        -Wall                    # Warning Level  TODO: Upgrade to -Wall -Wextra
        -Wno-overloaded-virtual  # TODO: Remove
        -Werror                  # Treat warnings as errors
    )

else()
    message(WARNING "Unsupported CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
endif()

# ~ User options ~
## ENABLE_CHECKED_CASTS
if(ENABLE_CHECKED_CASTS)
    target_compile_definitions(core PUBLIC CHG_ENABLE_CHECKED_CASTS)
endif()

## ENABLE_CHECKED_CASTS
if(ENABLE_CONCURRENCY_CHECKS)
    target_compile_definitions(core PUBLIC CHG_ENABLE_CONCURRENCY_CHECKS)
endif()

## LOG_LEVEL
if(LOG_LEVEL STREQUAL "DEFAULT")
    target_compile_definitions(core PUBLIC CHG_LOG_DEFAULT)
elseif(LOG_LEVEL STREQUAL "ERROR")
    target_compile_definitions(core PUBLIC CHG_LOG_ERROR)
elseif(LOG_LEVEL STREQUAL "WARN")
    target_compile_definitions(core PUBLIC CHG_LOG_WARN)
elseif(LOG_LEVEL STREQUAL "SUCCESS")
    target_compile_definitions(core PUBLIC CHG_LOG_SUCCESS)
elseif(LOG_LEVEL STREQUAL "INFO")
    target_compile_definitions(core PUBLIC CHG_LOG_INFO)
elseif(LOG_LEVEL STREQUAL "DEBUG")
    target_compile_definitions(core PUBLIC CHG_LOG_DEBUG)
elseif(LOG_LEVEL STREQUAL "TRACE")
    target_compile_definitions(core PUBLIC CHG_LOG_TRACE)
else()
    message(FATAL_ERROR "Unsupported LOG_LEVEL: ${LOG_LEVEL} (supported options: DEFAULT | ERROR | WARN | SUCCESS | INFO | DEBUG | TRACE)")
endif()

## EXTENDED_CHAR_SET
if(EXTENDED_CHAR_SET STREQUAL "CP437")
    target_compile_definitions(core PUBLIC CHG_EXTENDED_CHAR_SET_CP437)
elseif(EXTENDED_CHAR_SET STREQUAL "ASCII")
    target_compile_definitions(core PUBLIC CHG_EXTENDED_CHAR_SET_ASCII)
else()
    message(FATAL_ERROR "Unsupported EXTENDED_CHAR_SET: ${EXTENDED_CHAR_SET} (supported options: CP437 | ASCII)")
endif()

## COLOR_PRINT_MODE
if(COLOR_PRINT_MODE STREQUAL "ESCSEQ")
    target_compile_definitions(core PUBLIC CHG_COLOR_PRINT_MODE_ESCSEQ)
elseif(COLOR_PRINT_MODE STREQUAL "WINAPI")
    target_compile_definitions(core PUBLIC CHG_COLOR_PRINT_MODE_WINAPI)
elseif(COLOR_PRINT_MODE STREQUAL "DISABLED")
    target_compile_definitions(core PUBLIC CHG_COLOR_PRINT_MODE_DISABLED)
else()
    message(FATAL_ERROR "Unsupported COLOR_PRINT_MODE: ${COLOR_PRINT_MODE} (supported options: ESCSEQ | WINAPI | DISABLED)")
endif()

# ~ Thirdparty ~
## boost
FetchContent_Declare(boost
    URL ${BOOST_DOWNLOAD_URL}
    USES_TERMINAL_DOWNLOAD TRUE
    GIT_PROGRESS TRUE
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    DOWNLOAD_NO_EXTRACT FALSE
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(boost)

target_include_directories(core PUBLIC ${boost_SOURCE_DIR})

## cxxopts
message(STATUS "Fetching cxxopts v${CXXOPTS_VERSION} from ${CXXOPTS_REPOSITORY}")
FetchContent_Declare(cxxopts
    GIT_REPOSITORY ${CXXOPTS_REPOSITORY}
    GIT_TAG "v${CXXOPTS_VERSION}"
    GIT_SHALLOW ON
    GIT_PROGRESS TRUE
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(cxxopts)

target_link_libraries(core PUBLIC cxxopts)

## fmt
# message(STATUS "Fetching fmt v${FMT_VERSION} from ${FMT_REPOSITORY}")
# FetchContent_Declare(fmt
#     GIT_REPOSITORY ${FMT_REPOSITORY}
#     GIT_TAG ${FMT_VERSION}
#     GIT_SHALLOW ON
#     GIT_PROGRESS TRUE
#     EXCLUDE_FROM_ALL
#     SYSTEM
# )
# FetchContent_MakeAvailable(fmt)

# target_link_libraries(core PUBLIC fmt::fmt)

## SFML
message(STATUS "Fetching SFML v${SFML_VERSION} from ${SFML_REPOSITORY}")
FetchContent_Declare(SFML
    GIT_REPOSITORY ${SFML_REPOSITORY}
    GIT_TAG ${SFML_VERSION}
    GIT_SHALLOW ON
    GIT_PROGRESS TRUE
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

target_link_libraries(core PUBLIC SFML::Graphics)
