file(GLOB THREADPOOL_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/threadpool/*.cpp")

# ###########################################################################################
# make threadpool
# ###########################################################################################
foreach(threadpool_test_source ${THREADPOOL_TEST_SOURCES})
    get_filename_component(threadpool_test_filename ${threadpool_test_source} NAME)
    string(REPLACE ".cpp" "" threadpool_test_name ${threadpool_test_filename})
    add_executable(${threadpool_test_name} ${threadpool_test_source})
    add_dependencies(build-tests ${threadpool_test_name})
    add_dependencies(check-tests ${threadpool_test_name})
    target_link_libraries(${threadpool_test_name} web_shared pthread mysqlclient)
    set_target_properties(${threadpool_test_name}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
    )
endforeach(threadpool_test_source ${THREADPOOL_TEST_SOURCES})
