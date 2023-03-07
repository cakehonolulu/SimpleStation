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

if (DUMP_VRAM)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDUMP_VRAM")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDUMP_VRAM")
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -DDUMP_VRAM")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -DDUMP_VRAM")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DDUMP_VRAM")
endif()

if (ASAN)
add_compile_options(-fsanitize=address -g)
add_link_options(-fsanitize=address)
endif()

find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)

include_directories(${VULKAN_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} "include/")


option(USE_GCC "Use GCC C Compiler instead of LLVMs Clang" OFF)
option(TESTS "Build SimpleStation in Unit Test Mode" OFF)


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

set (EXCLUDE_DIR "/external/")
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


file(GLOB GLSL_SOURCE_FILES
    "frag_shader.frag"
    "vert_shader.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME_WE)
  set(SPIRV "${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND glslangValidator -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(glsl_compile DEPENDS ${SPIRV_BINARY_FILES})

add_dependencies(simplestation glsl_compile)

target_link_libraries(simplestation glfw ${SDL2_LIBRARIES} cglm_headers)

add_subdirectory(external/cglm/ EXCLUDE_FROM_ALL)

if (APPLE)
  target_link_libraries(simplestation "-framework OpenGL" Vulkan::Vulkan GLEW::GLEW)
elseif(NOT APPLE AND NOT WIN32)
  target_link_libraries(simplestation ${GLEW_LIBRARIES} vulkan)
else(NOT APPLE AND NOT UNIX)
  target_link_libraries(simplestation ${GLEW_LIBRARIES} vulkan-1)
endif()
