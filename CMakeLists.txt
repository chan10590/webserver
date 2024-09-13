# #####################################################################################################################
# DEPENDENCIES
# #####################################################################################################################
cmake_minimum_required(VERSION 3.10)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(HttpServer
        VERSION 0.1
        DESCRIPTION "HttpServer"
        LANGUAGES C CXX
)

# #####################################################################################################################
# DEPENDENCIES
# #####################################################################################################################
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" PATH_TO_CMAKELISTS_TXT)

if(EXISTS "${PATH_TO_CMAKELISTS_TXT}")
        message(FATAL_ERROR "Run CMake from a build subdirectory! \"mkdir build ; cd build ; cmake .. \" \
    Some junk files were created in this folder (CMakeCache.txt, CMakeFiles); you should delete those.")
endif()

# #####################################################################################################################
# COMPILER SETUP
# #####################################################################################################################
# -fPIC:                          生成位置无关代码，对于构建共享库是必需的。
# -Wall:                          启用所有常见的编译警告。
# -Wextra:                        启用一些额外的警告
# -Wno-unused-parameter:          禁用对未使用参数的警告。
# -Wno-attributes:                禁用对属性的警告。
# -O0:                            关闭所有优化，以便更好地进行调试。
# -ggdb:                          生成用于 gdb 调试的信息。
# -fno-omit-frame-pointer:        保留帧指针以便更好地进行栈跟踪。
# -fno-optimize-sibling-calls:    禁用尾调用优化，以便更准确地调试调用栈。
# #####################################################################################################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra -std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-attributes")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIC")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fPIC")
set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fPIC")
set(GCC_COVERAGE_LINK_FLAGS "-fPIC")
set(CMAKE_BUILD_TYPE Debug) # coredump调试

# Output directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Includes.
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(INCLUDE_EVENTDRIVER_DIR ${PROJECT_SOURCE_DIR}/include/eventdriver)
set(INCLUDE_LOG_DIR ${PROJECT_SOURCE_DIR}/include/log)
set(INCLUDE_HTTPSERVICE_DIR ${PROJECT_SOURCE_DIR}/include/httpservice)
set(INCLUDE_SERVER_DIR ${PROJECT_SOURCE_DIR}/include/server)
include_directories(
        ${INCLUDE_DIR}
        ${INCLUDE_EVENTDRIVER_DIR}
        ${INCLUDE_LOG_DIR}
        ${INCLUDE_HTTPSERVICE_DIR}
        ${INCLUDE_SERVER_DIR})

# 设置子目录，分开构建
add_subdirectory(src)
add_subdirectory(test)
