project(aad_library)

set(includes includes/)
set(sources src/aad_library.cpp

)

add_library(${PROJECT_NAME} ${sources})
target_include_directories(${PROJECT_NAME} PUBLIC ${includes})