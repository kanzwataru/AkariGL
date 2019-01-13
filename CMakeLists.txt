cmake_minimum_required(VERSION 3.1)

project(AkariGL)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_C_FLAGS "-O3")

file(GLOB_RECURSE SRCS src/*.c extern/*.c)
file(GLOB_RECURSE HDRS src/*.h extern/*.h)

add_executable(${PROJECT_NAME} ${SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/extern {CMAKE_SOURCE_DIR}/extern/glad/include)

include_directories(extern/glad/include)
target_link_libraries(${PROJECT_NAME} dl SDL2)
target_sources(${PROJECT_NAME} PRIVATE ${HDRS})
