add_library(hittable_list INTERFACE)
target_include_directories(hittable_list INTERFACE ./)
target_link_libraries(hittable_list INTERFACE hittable)
