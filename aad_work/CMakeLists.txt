project(aad_work)

set(includes ./ ../aad_library/)

add_executable(my_test my_test.cpp)
target_include_directories(my_test PUBLIC ${includes})
target_link_libraries(my_test aad_library)