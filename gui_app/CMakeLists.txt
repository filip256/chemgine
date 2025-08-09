file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

add_executable(gui_app ${SOURCES})

# Generate VS filters based on dir structure.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

target_include_directories(gui_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(gui_app PRIVATE core)

vs_set_cwd_to_target(gui_app)

copy_dir_to_target(gui_app "${CMAKE_CURRENT_SOURCE_DIR}/data" "data")
copy_dir_to_target(gui_app "${CMAKE_CURRENT_SOURCE_DIR}/fonts" "fonts")
