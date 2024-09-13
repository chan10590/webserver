file(GLOB_RECURSE SRC_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_library(web_shared ${SRC_FILES})

file(GLOB_RECURSE INCLUDE_DIRS RELATIVE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/*)

foreach(subdir ${INCLUDE_DIRS})
    if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/include/${subdir})
        target_include_directories(web_shared PUBLIC ${PROJECT_SOURCE_DIR}/include/${subdir})
    endif()
endforeach()
