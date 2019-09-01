cmake_minimum_required(VERSION 3.1)

project(AkariGL)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

file(GLOB_RECURSE SRCS src/*.c extern/*.c)
file(GLOB_RECURSE HDRS src/*.h extern/*.h)
file(GLOB_RECURSE RES res/*)

# remove unneeded things from cglm
list(FILTER SRCS EXCLUDE REGEX "extern\/.*dllmain.c$")
list(FILTER SRCS EXCLUDE REGEX "extern\/.*test.*")
list(FILTER HDRS EXCLUDE REGEX "extern\/.*test.*")
list(FILTER SRCS EXCLUDE REGEX "extern\/.*examples.*")
list(FILTER HDRS EXCLUDE REGEX "extern\/.*examples.*")
# ##

add_executable(${PROJECT_NAME} ${SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src {CMAKE_SOURCE_DIR}/extern/glad/include {CMAKE_SOURCE_DIR}/extern/cglm/include {CMAKE_SOURCE_DIR}/extern/tinyobjloader-c)

include_directories(extern/glad/include extern/cglm/include extern/stb extern/tinyobjloader-c)
target_link_libraries(${PROJECT_NAME} dl SDL2 m)
target_sources(${PROJECT_NAME} PRIVATE ${HDRS})

set(res_outputs "")
foreach(rfile ${RES})
    file(RELATIVE_PATH fname ${CMAKE_SOURCE_DIR}/res/ ${rfile})
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/bin/res/${fname}
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/res/${fname}
            ${CMAKE_BINARY_DIR}/bin/res/${fname}
        DEPENDS ${CMAKE_SOURCE_DIR}/res/${fname})
    list(APPEND res_outputs ${CMAKE_BINARY_DIR}/bin/res/${fname})
endforeach()

add_custom_target(
    copy_res ALL
    DEPENDS ${res_outputs}
    COMMENT "Copying resource files"
)
