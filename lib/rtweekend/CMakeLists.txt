add_library(rtweekend INTERFACE)
target_include_directories(rtweekend INTERFACE ./)

target_link_libraries(rtweekend INTERFACE ray vec3 interval)

