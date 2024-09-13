file(GLOB CLIENT_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/client/*.cpp")

# ###########################################################################################
# make client(如果client下有其他.cpp，还可执行其他make操作)
# ###########################################################################################
foreach(client_test_source ${CLIENT_TEST_SOURCES})
    get_filename_component(client_test_filename ${client_test_source} NAME)
    string(REPLACE ".cpp" "" client_test_name ${client_test_filename})
    add_executable(${client_test_name} EXCLUDE_FROM_ALL ${client_test_source})
    add_dependencies(build-tests ${client_test_name})
    add_dependencies(check-tests ${client_test_name})
    target_link_libraries(${client_test_name} web_shared pthread mysqlclient)
    set_target_properties(${client_test_name}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
        COMMAND ${client_test_name}
    )
endforeach(client_test_source ${CLIENT_TEST_SOURCES})
