add_library(color INTERFACE)
target_include_directories(color INTERFACE ./)
target_link_libraries(color INTERFACE vec3)
