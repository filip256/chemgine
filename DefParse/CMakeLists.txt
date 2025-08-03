file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

add_executable(defparse ${SOURCES})

target_include_directories(defparse PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(defparse PRIVATE core)

vs_set_cwd_to_target(defparse)
