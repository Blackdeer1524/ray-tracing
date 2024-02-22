cmake_minimum_required(VERSION 3.22)
project(coursework)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)

add_executable(main main.cpp)
target_link_libraries(main vec3)
target_link_libraries(main color)
target_link_libraries(main ray)
target_link_libraries(main sphere)
target_link_libraries(main rtweekend)
target_link_libraries(main hittable)

