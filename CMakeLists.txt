cmake_minimum_required(VERSION 3.22)
project(hypersnake)

set(CMAKE_CXX_STANDARD 14)
set(SDL2_INCLUDE_DIR D:\\SDL\\include)
set(SDL2_LIB_DIR D:\\SDL\\lib\\x64)

include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})

add_executable(hypersnake main.cpp)

target_link_libraries(${PROJECT_NAME} SDL2main SDL2 SDL2_gfx)