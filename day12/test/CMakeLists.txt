file(GLOB TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*.cpp")

# ---------------------------------------------------------------------------------------
# make server client ...
# ---------------------------------------------------------------------------------------
foreach (test_source ${TEST_SOURCES})
    # Create a human readable name.
    get_filename_component(test_filename ${test_source} NAME)
    string(REPLACE ".cpp" "" test_name ${test_filename})

    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} tserver_shared)

endforeach(test_source ${TEST_SOURCES})