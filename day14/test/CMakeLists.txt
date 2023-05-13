file(GLOB TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*.cpp")

set(CMAKE_THREAD_PREFER_PTHREAD TRUE) # 优先使用pthread
set(THREADS_PREFER_PTHREAD_FLAG TRUE) # 添加-pthread标志
find_package(Threads REQUIRED)

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