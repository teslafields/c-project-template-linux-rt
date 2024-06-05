cmake_minimum_required(VERSION 3.21)

#
# Set the project name and version
#
project(controloop VERSION 1.0)

set(APP_NAME ${PROJECT_NAME}_app)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)

#
# Check cross compile toolchain
#
set(CROSS_COMPILE $ENV{CROSS_COMPILE})
if(CROSS_COMPILE)
    message(STATUS "CROSS_COMPILE for: " ${CROSS_COMPILE})

    set(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc${TOOLCHAIN_EXT})
    set(CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++${TOOLCHAIN_EXT})
    set(CMAKE_ASM_COMPILER ${CROSS_COMPILE}gcc${TOOLCHAIN_EXT})
    set(CMAKE_OBJCOPY ${CROSS_COMPILE}objcopy CACHE INTERNAL "objcopy tool")

    # This damn clang-tidy thing complains when cross compiling
    # add_compile_options("$<$<C_COMPILER_ID:Clang>:-target=aarch64-fslc-linux")
else()
    message(STATUS "CROSS_COMPILE not set, build is native")
    set(CMAKE_C_CLANG_TIDY clang-tidy)
endif()

#
# Compiler options
#
add_compile_options(
    -Wall
    -Wextra
    -Wcast-qual
    -Wconversion
    -Wno-sign-conversion
    -g3
    -O0
    $<$<CONFIG:RELEASE>:-g0> # overwrites -g3
    $<$<CONFIG:RELEASE>:-O3> # overwrites -O0
    $<$<CONFIG:ASAN>:-fsanitize=address>
    $<$<CONFIG:ASAN>:-fno-omit-frame-pointer>
    $<$<CONFIG:UBSAN>:-fsanitize=undefined>
    $<$<CONFIG:UBSAN>:-fno-omit-frame-pointer>
    $<$<CONFIG:TSAN>:-fsanitize=thread>)

add_link_options(
    -pthread $<$<CONFIG:ASAN>:-fsanitize=address>
    $<$<CONFIG:UBSAN>:-fsanitize=undefined> $<$<CONFIG:TSAN>:-fsanitize=thread>)

#
# Add `DEBUG` symbol when not in release mode
#
add_compile_definitions($<$<NOT:$<CONFIG:RELEASE>>:DEBUG>)

#
# Build Libraries
#
add_subdirectory(src)

#
# Build the Executable
#
add_executable(${APP_NAME} apps/main.c)
target_link_libraries(${APP_NAME} PUBLIC ${PROJECT_NAME})


if(NOT CROSS_COMPILE)
    # Set capabilities for thread RT configuration
    add_custom_command(TARGET ${APP_NAME} POST_BUILD COMMAND sudo setcap CAP_SYS_NICE+eip 
        ${CMAKE_SOURCE_DIR}/build/${APP_NAME}
    )
endif()

#
# Build Tests
#
enable_testing()
add_subdirectory(tests)
