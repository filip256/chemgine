file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

add_library(core STATIC ${SOURCES})

# Generate VS filters based on dir structure.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# boost
message(STATUS "BOOST_INCLUDE_DIR = ${BOOST_INCLUDE_DIR}")
target_include_directories(core PUBLIC ${BOOST_INCLUDE_DIR})

# SFML
message(STATUS "SFML_INCLUDE_DIR = ${SFML_INCLUDE_DIR}")
message(STATUS "SFML_LIB_DIR = ${SFML_LIB_DIR}")

target_compile_definitions(core PUBLIC SFML_STATIC)
target_include_directories(core PUBLIC ${SFML_INCLUDE_DIR})
target_link_directories(core PUBLIC ${SFML_LIB_DIR})
target_link_libraries(core PUBLIC
    debug sfml-graphics-s-d optimized sfml-graphics-s
    debug sfml-window-s-d   optimized sfml-window-s
    debug sfml-system-s-d   optimized sfml-system-s
    freetype
    opengl32
    winmm
)

# cxxopts
message(STATUS "CXXOPTS_INCLUDE_DIR = ${CXXOPTS_INCLUDE_DIR}")
target_include_directories(core PUBLIC ${CXXOPTS_INCLUDE_DIR})
