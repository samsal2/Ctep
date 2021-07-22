cmake_minimum_required(VERSION 3.17.3)

project(ctep)

set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
        ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}//main.cpp)

target_compile_options(${PROJECT_NAME} PRIVATE -O3 
                                               -Wall
                                               -Wextra
                                               -Werror
                                               -Wpedantic
                                               -pedantic-errors
                                               -Wshadow)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/)
