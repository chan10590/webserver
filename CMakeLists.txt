cmake_minimum_required(VERSION 3.10) # Cmake最低版本要求
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 生成 compile_commands.json 文件，对于工具如 clang-tidy 和 clang-format 非常有用。
set(BUILD_SHARED_LIBS ON) # 指定默认构建共享库（动态库）
set(CMAKE_CXX_STANDARD 17) # C++17标准
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 确保强制使用 C++17 标准。如果编译器不支持 C++17，构建将失败并显示错误信息。

project(Pine # 项目名称，自己指定
        VERSION 0.1 # 项目版本
        DESCRIPTION "pine" # 项目描述
        LANGUAGES C CXX # 项目使用的语言
)

# People keep running CMake in the wrong folder, completely nuking their project or creating weird bugs.
# This checks if you're running CMake from a folder that already has CMakeLists.txt.
# Importantly, this catches the common case of running it from the root directory.

# 将 ${PROJECT_BINARY_DIR}/CMakeLists.txt 路径转换为适合 CMake 使用的路径格式，并将结果存储在 PATH_TO_CMAKELISTS_TXT 变量中。
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" PATH_TO_CMAKELISTS_TXT)

# 检查CMakeLists.txt 文件是否存在于 ${PROJECT_BINARY_DIR} 目录中。
if(EXISTS "${PATH_TO_CMAKELISTS_TXT}")
        # 输出错误信息，提示用户新建build文件夹用于cmake编译，防止源目录中的生成文件被污染
        message(FATAL_ERROR "Run CMake from a build subdirectory! \"mkdir build ; cd build ; cmake .. \" \
    Some junk files were created in this folder (CMakeCache.txt, CMakeFiles); you should delete those.")
endif()

# Expected directory structure.
# 设置构建支持目录。
set(PINE_BUILD_SUPPORT_DIR "${CMAKE_SOURCE_DIR}/build_support")

# 设置查找 clang-format 和 clang-tidy 的路径。
set(PINE_CLANG_SEARCH_PATH "/usr/local/bin" "/usr/bin" "/usr/local/opt/llvm/bin" "/usr/local/opt/llvm@8/bin" "/usr/local/Cellar/llvm/8.0.1/bin")

# #####################################################################################################################
# DEPENDENCIES
# #####################################################################################################################

# CTest
# enable_testing()

# clang-format
# 检查 CLANG_FORMAT_BIN 是否已定义
if(NOT DEFINED CLANG_FORMAT_BIN)
        # attempt to find the binary if user did not specify
        # 查找 clang-format 可执行文件
        find_program(CLANG_FORMAT_BIN # 存储找到的可执行文件路径
                NAMES clang-format clang-format-8 # 要搜索的可执行文件名
                HINTS ${PINE_CLANG_SEARCH_PATH}) # 搜索路径
endif()

# 检查是否找到 clang-format
if("${CLANG_FORMAT_BIN}" STREQUAL "CLANG_FORMAT_BIN-NOTFOUND")
        message(WARNING "Pine/main couldn't find clang-format.")
else()
        message(STATUS "Pine/main found clang-format at ${CLANG_FORMAT_BIN}")
endif()

# clang-tidy
if(NOT DEFINED CLANG_TIDY_BIN)
        # attempt to find the binary if user did not specify
        find_program(CLANG_TIDY_BIN
                NAMES clang-tidy clang-fidy-8
                HINTS ${PINE_CLANG_SEARCH_PATH})
endif()

if("${CLANG_TIDY_BIN}" STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
        message(WARNING "Pine/main couldn't find clang-tidy.")
else()
        # Output compile_commands.json
        # 如果找到了 clang-tidy，则设置 CMAKE_EXPORT_COMPILE_COMMANDS 变量为 1，这个选项会使 CMake 生成 compile_commands.json 文件，该文件用于提供编译信息给代码分析工具
        set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
        message(STATUS "Pine/main found clang-fidy at ${CLANG_TIDY_BIN}")
endif()

# cpplint
find_program(CPPLINT_BIN
        NAMES cpplint cpplint.py
        HINTS ${PINE_BUILD_SUPPORT_DIR}
        PATHS /usr/local/bin /usr/bin)

if("${CPPLINT_BIN}" STREQUAL "CPPLINT_BIN-NOTFOUND")
        message(WARNING "Pine/main couldn't find cpplint.")
else()
        message(STATUS "Pine/main found cpplint at ${CPPLINT_BIN}")
endif()

# #####################################################################################################################
# COMPILER SETUP
# #####################################################################################################################

# Compiler flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra -std=c++17 -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-attributes") # TODO: remove
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIC")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fPIC")
set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fPIC")

set(GCC_COVERAGE_LINK_FLAGS "-fPIC")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")

# Output directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # 设置静态库（.a 文件）的输出目录。
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # 设置共享库（.so 或 .dll 文件）的输出目录。
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # 设置可执行文件的输出目录。

# Includes.
set(PINE_SRC_INC_DIR ${PROJECT_SOURCE_DIR}/src/inc) # 设置源代码的头文件目录。
set(PINE_TEST_INC_DIR ${PROJECT_SOURCE_DIR}/test/inc) # 设置测试代码的头文件目录。
include_directories(${PINE_SRC_INC_DIR} ${PINE_TEST_INC_DIR}) # 添加头文件目录，以便编译器在这些目录中查找头文件。

# 设置子目录，分开构建
add_subdirectory(src)
add_subdirectory(test)

# #####################################################################################################################
# MAKE TARGETS
# #####################################################################################################################

# #########################################
# "make format"
# "make check-format"
# #########################################
# 使用 clang-format 来格式化源代码，将需要格式化的目录路径连接起来，并将结果赋值给 PINE_FORMAT_DIRS 变量
string(CONCAT PINE_FORMAT_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/src,"
        "${CMAKE_CURRENT_SOURCE_DIR}/test,"
)

# runs clang format and updates files in place.
add_custom_target(format ${PINE_BUILD_SUPPORT_DIR}/run_clang_format.py # clang-format 的脚本路径。
        ${CLANG_FORMAT_BIN} # clang-format 的可执行文件路径
        ${PINE_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt # 包含要排除的文件或目录的排除文件（不用clang-format格式化的文件）。
        --source_dirs ${PINE_FORMAT_DIRS} # 指定要格式化的源代码目录
        --fix # 指示 clang-format 直接修复格式化问题
        --quiet # 抑制 clang-format 的输出，只显示错误或警告
)

# runs clang format and exits with a non-zero exit code if any files need to be reformatted
# 检查源代码是否需要格式化
add_custom_target(check-format ${PINE_BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${PINE_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${PINE_FORMAT_DIRS}
        --quiet
)

# #########################################
# "make cpplint"
# #########################################
# 递归地收集要进行 lint 检查的文件
file(GLOB_RECURSE PINE_LINT_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
)

# Balancing act: cpplint.py takes a non-trivial time to launch,
# so process 12 files per invocation, while still ensuring parallelism
add_custom_target(cpplint echo '${PINE_LINT_FILES}' | xargs -n12 -P8
        ${CPPLINT_BIN}
        --verbose=2 --quiet
        --linelength=120
        --filter=-legal/copyright,-build/include_subdir,-readability/casting
)

# ##########################################################
# "make clang-tidy" target
# ##########################################################
# runs clang-tidy and exits with a non-zero exit code if any errors are found.
# note that clang-tidy automatically looks for a .clang-tidy file in parent directories
add_custom_target(clang-tidy
        ${PINE_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN} # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR} # using cmake's generated compile commands
)

# add_dependencies(check-clang-tidy pine_shared)                    # needs gtest headers, compile_commands.json
