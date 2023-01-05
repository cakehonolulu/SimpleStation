cmake_minimum_required(VERSION 3.3)
project(SimpleStation C)

set(CMAKE_C_FLAGS_DEBUG "-Wall -Wextra -g")
set(CMAKE_C_FLAGS_MINSIZEREL "-Wall -Wextra -O0")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-Wall -Wextra -g -O2")
set(CMAKE_C_FLAGS_RELEASE "-Wall -Wextra -O2")

if (DEBUG_CPU)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG_CPU")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG_CPU")
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -DDEBUG_CPU")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -DDEBUG_CPU")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DDEBUG_CPU")
endif()

find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})


option(USE_GCC "Use GCC C Compiler instead of LLVMs Clang" OFF)
option(TESTS "Build SimpleStation in Unit Test Mode" OFF)


include_directories("include/")

if(USE_GCC)
endif()

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/*.c)

set (EXCLUDE_DIR "/CMakeFiles/")
file (GLOB_RECURSE SRC_FILES "*.cpp" "*.c")
foreach (TMP_PATH ${SRC_FILES})
    string (FIND ${TMP_PATH} ${EXCLUDE_DIR} EXCLUDE_DIR_FOUND)
    if (NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
        list (REMOVE_ITEM SRC_FILES ${TMP_PATH})
    endif ()
endforeach(TMP_PATH)

if(TESTS)
list(REMOVE_ITEM SRC_FILES ${CMAKE_SOURCE_DIR}/simplestation.c)
else(NOT TESTS)
list(REMOVE_ITEM SRC_FILES ${CMAKE_SOURCE_DIR}/tests/test.c)
endif()


add_executable(simplestation ${SRC_FILES})
target_link_libraries(simplestation glfw ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

if (APPLE)
  target_link_libraries(simplestation "-framework OpenGL" GLEW::GLEW)
else(NOT APPLE)
  target_link_libraries(simplestation ${GLEW_LIBRARIES})
endif()
