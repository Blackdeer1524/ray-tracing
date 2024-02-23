cmake_minimum_required(VERSION 3.22)
project(coursework)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_subdirectory(lib)

add_executable(main main.cpp)
target_link_libraries(main vec3)
target_link_libraries(main color)
target_link_libraries(main ray)
target_link_libraries(main sphere)
target_link_libraries(main rtweekend)
target_link_libraries(main hittable)
target_link_libraries(main hittable_list)
target_link_libraries(main camera)


