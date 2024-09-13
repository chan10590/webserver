file(GLOB LOG_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/log/*.cpp")

# ###########################################################################################
# make log
# ###########################################################################################
foreach(log_test_source ${LOG_TEST_SOURCES})
    get_filename_component(log_test_filename ${log_test_source} NAME)
    string(REPLACE ".cpp" "" log_test_name ${log_test_filename})
    add_executable(${log_test_name} ${log_test_source})
    add_dependencies(build-tests ${log_test_name})
    add_dependencies(check-tests ${log_test_name})
    target_link_libraries(${log_test_name} web_shared pthread mysqlclient)
    set_target_properties(${log_test_name}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
    )
endforeach(log_test_source ${LOG_TEST_SOURCES})
