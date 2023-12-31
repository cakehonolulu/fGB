cmake_minimum_required(VERSION 3.5)
project(fGB)

include(FindPkgConfig)

pkg_check_modules (CAPSTONE REQUIRED capstone)

set(CMAKE_C_FLAGS_DEBUG "-Wall -Wextra -g")
set(CMAKE_C_FLAGS_MINSIZEREL "-Wall -Wextra -O0")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-Wall -Wextra -g -O2")
set(CMAKE_C_FLAGS_RELEASE "-Wall -Wextra -O2")

if (ASAN)
add_compile_options(-fsanitize=address -g)
add_link_options(-fsanitize=address)
endif()

add_compile_options(-g -Iinclude -Iexternal/zbyak -Wall -Wextra -std=c++2b)

if(${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    add_compile_options(-DJIT_AMD64)
endif()

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/*.c)

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/*.c)

set (EXCLUDE_DIR "/CMakeFiles/")
file (GLOB_RECURSE SRC_FILES "*.cpp" "*.cxx" "*.cc")
foreach (TMP_PATH ${SRC_FILES})
    string (FIND ${TMP_PATH} ${EXCLUDE_DIR} EXCLUDE_DIR_FOUND)
    if (NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
        list (REMOVE_ITEM SRC_FILES ${TMP_PATH})
    endif ()
endforeach(TMP_PATH)

set (EXCLUDE_DIR "/external/")
foreach (TMP_PATH ${SRC_FILES})
    string (FIND ${TMP_PATH} ${EXCLUDE_DIR} EXCLUDE_DIR_FOUND)
    if (NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
        list (REMOVE_ITEM SRC_FILES ${TMP_PATH})
    endif ()
endforeach(TMP_PATH)

add_executable(fgb ${SRC_FILES})
target_link_libraries(fgb ${CAPSTONE_LIBRARIES})
