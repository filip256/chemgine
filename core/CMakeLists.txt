file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

add_library(core STATIC ${SOURCES})

# Generate VS filters based on dir structure.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Compiler Options
if(MSVC)
    target_compile_options(core PUBLIC
        /W3  # Warning Level  # TODO: Upgrade to W4
        /WX  # Treat warnings as errors
    )
endif()

# boost
message(STATUS "BOOST_INCLUDE_DIR = ${BOOST_INCLUDE_DIR}")
target_include_directories(core PUBLIC ${BOOST_INCLUDE_DIR})

# cxxopts
message(STATUS "CXXOPTS_INCLUDE_DIR = ${CXXOPTS_INCLUDE_DIR}")
target_include_directories(core PUBLIC ${CXXOPTS_INCLUDE_DIR})

# SFML
message(STATUS "Fetching SFML ${SFML_VERSION} from ${SFML_REPOSITORY}")
FetchContent_Declare(SFML
    GIT_REPOSITORY ${SFML_REPOSITORY}
    GIT_TAG ${SFML_VERSION}
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

target_link_libraries(core PUBLIC SFML::Graphics)
