file(GLOB SERVER_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/server/*.cpp")

# ###########################################################################################
# make server
# ###########################################################################################
foreach(server_test_source ${SERVER_TEST_SOURCES})
    get_filename_component(server_test_filename ${server_test_source} NAME)
    string(REPLACE ".cpp" "" server_test_name ${server_test_filename})
    add_executable(${server_test_name} ${server_test_source})
    add_dependencies(build-tests ${server_test_name})
    add_dependencies(check-tests ${server_test_name})
    target_link_libraries(${server_test_name} web_shared pthread mysqlclient)
    set_target_properties(${server_test_name}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
    )
endforeach(server_test_source ${SERVER_TEST_SOURCES})
