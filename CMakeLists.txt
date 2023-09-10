cmake_minimum_required(VERSION 3.5)
project(fGB C)

set(CMAKE_C_FLAGS_DEBUG "-Wall -Wextra -g")
set(CMAKE_C_FLAGS_MINSIZEREL "-Wall -Wextra -O0")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-Wall -Wextra -g -O2")
set(CMAKE_C_FLAGS_RELEASE "-Wall -Wextra -O2")

option(ZYDIS_BUILD_TOOLS "" OFF)
option(ZYDIS_BUILD_EXAMPLES "" OFF)
option(ZYDIS_BUILD_MAN "" OFF)
option(ZYDIS_BUILD_DOXYGEN "" OFF)
add_subdirectory("external/zydis")

if (ASAN)
add_compile_options(-fsanitize=address -g)
add_link_options(-fsanitize=address)
endif()

add_compile_options(-g -Iinclude -std=c2x -Wall -Wextra)

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/*.c)

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/*.c)

set (EXCLUDE_DIR "/CMakeFiles/")
file (GLOB_RECURSE SRC_FILES "*.cpp" "*.c")
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
target_link_libraries(fgb PRIVATE Zydis)
