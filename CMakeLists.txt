cmake_minimum_required(VERSION 3.1)

project(AkariGL)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

file(GLOB_RECURSE SRCS src/*.c extern/*.c)
file(GLOB_RECURSE HDRS src/*.h extern/*.h)

# remove unneeded things from cglm
list(FILTER SRCS EXCLUDE REGEX "extern\/.*dllmain.c$")
list(FILTER SRCS EXCLUDE REGEX "extern\/.*test.*")
list(FILTER HDRS EXCLUDE REGEX "extern\/.*test.*")
# ##

add_executable(${PROJECT_NAME} ${SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src {CMAKE_SOURCE_DIR}/extern/glad/include {CMAKE_SOURCE_DIR}/extern/cglm/include)

include_directories(extern/glad/include extern/cglm/include extern/stb)
target_link_libraries(${PROJECT_NAME} dl SDL2 m)
target_sources(${PROJECT_NAME} PRIVATE ${HDRS})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)
