cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PRJ_NAME CHIP8)

project(${PRJ_NAME})

set(SOURCE_DIR src)
set(INCLUDE_DIR inc)

set(SRC_FILES
    ${SOURCE_DIR}/handleOpcode.cpp
    ${SOURCE_DIR}/machine.cpp
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/opcodes.cpp
    )

add_executable(${PRJ_NAME} ${SRC_FILES})
target_include_directories(${PRJ_NAME} PUBLIC ${INCLUDE_DIR})

# SDL2
find_package(SDL2 REQUIRED)
target_link_libraries(${PRJ_NAME} PUBLIC SDL2 SDL2main)
target_include_directories(${PRJ_NAME} PUBLIC ${SDL2_INCLUDE_DIRS}) # SDL2_INCLUDE_DIRS is already defined.