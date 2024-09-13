add_custom_target(build-tests COMMAND ${CMAKE_CTEST_COMMAND} --show-only)
add_custom_target(check-tests COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

add_subdirectory(client)
add_subdirectory(log)
add_subdirectory(server)
add_subdirectory(threadpool)
