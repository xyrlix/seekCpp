# set property
set_property(THIS_APP_NAME "todo_list")

# Add executable
add_executable(THIS_APP_NAME
    src/main.cpp
)

# Set output directory
set_target_properties(THIS_APP_NAME PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/THIS_APP_NAME
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/THIS_APP_NAME
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/THIS_APP_NAME
)

# Add tests if testing is enabled
if(BUILD_TESTING)
    add_subdirectory(test)
endif()