cmake_minimum_required(VERSION 3.28)
project(abel)

add_library(libabel SHARED src/abel.cpp)
target_include_directories(libabel PUBLIC src)
set_target_properties(libabel PROPERTIES OUTPUT_NAME abel)

add_executable(abel src/main.cpp)
target_link_libraries(abel PRIVATE libabel)
