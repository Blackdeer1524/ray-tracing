add_library(sphere INTERFACE)
target_include_directories(sphere INTERFACE ./)
target_link_libraries(sphere INTERFACE hittable)
