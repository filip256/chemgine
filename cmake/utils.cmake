# Copies the contents of a given directory to a given destination inside the target directory.
# The destination directory will be created if it doesn;t aready exist.
function(copy_dir_to_target target_name src dst_name)
    set(dst "$<TARGET_FILE_DIR:${target_name}>/${dst_name}")

    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${dst}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${src}" "${dst}"
        VERBATIM
    )
endfunction()

# Sets the working directory for the VisualStudio debugger to the target directory.
function(vs_set_cwd_to_target target_name)
    set_target_properties(${target_name} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${target_name}>"
    )
endfunction()
