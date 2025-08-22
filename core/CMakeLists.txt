file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

add_library(core STATIC ${SOURCES})

# Generate VS filters based on dir structure.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Compiler options
if(MSVC)
    target_compile_options(core PUBLIC
        /W3     # Warning Level  TODO: Upgrade to /W4
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

# User options
if(DISABLE_CHECKED_CASTS)
    target_compile_definitions(core CHG_DISABLE_CHECKED_CASTS)
endif()

if(DISABLE_CONCURRENCY_CHECKS)
    target_compile_definitions(core PUBLIC CHG_DISABLE_CONCURRENCY_CHECKS)
endif()

if(DISABLE_COLORED_PRINTING)
    target_compile_definitions(core PUBLIC CHG_DISABLE_COLORED_PRINTING)
endif()

if(EXTENDED_CHAR_SET STREQUAL "CP437")
    target_compile_definitions(core PUBLIC CHG_EXTENDED_CHAR_SET_CP437)
elseif(EXTENDED_CHAR_SET STREQUAL "ASCII")
    target_compile_definitions(core PUBLIC CHG_EXTENDED_CHAR_SET_ASCII)
else()
    message(FATAL_ERROR "Unsupported EXTENDED_CHAR_SET: ${EXTENDED_CHAR_SET} (supported options: CP437 | ASCII)")
endif()

# boost
target_include_directories(core PUBLIC ${BOOST_INCLUDE_DIR})

# cxxopts
message(STATUS "Fetching cxxopts ${CXXOPTS_VERSION} from ${CXXOPTS_REPOSITORY}")
FetchContent_Declare(cxxopts
    GIT_REPOSITORY ${CXXOPTS_REPOSITORY}
    GIT_TAG ${CXXOPTS_VERSION}
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(cxxopts)

target_link_libraries(core PUBLIC cxxopts)

# fmt
message(STATUS "Fetching fmt v${FMT_VERSION} from ${FMT_REPOSITORY}")
FetchContent_Declare(fmt
    GIT_REPOSITORY ${FMT_REPOSITORY}
    GIT_TAG ${FMT_VERSION}
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(fmt)

target_link_libraries(core PUBLIC fmt::fmt)

# SFML
message(STATUS "Fetching SFML v${SFML_VERSION} from ${SFML_REPOSITORY}")
FetchContent_Declare(SFML
    GIT_REPOSITORY ${SFML_REPOSITORY}
    GIT_TAG ${SFML_VERSION}
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

target_link_libraries(core PUBLIC SFML::Graphics)
