add_library(camera INTERFACE)
target_include_directories(camera INTERFACE ./)
target_link_libraries(camera INTERFACE common color hittable material)

