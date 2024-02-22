add_library(hittalbe_list INTERFACE)
target_include_directories(hittalbe_list INTERFACE ./)
target_link_libraries(hittalbe_list INTERFACE hittable)
