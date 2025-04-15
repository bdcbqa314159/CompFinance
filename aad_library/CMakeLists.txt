project(aad_library)

set(includes includes/)
set(sources src/aad_library.cpp
            src/AAD.cpp
            src/mcBase.cpp
            src/sobol.cpp
            src/ThreadPool.cpp
            src/xlcall.cpp
            src/xlframework.c
            src/xlMemoryManager.cpp
            src/xlMemoryPool.cpp

)

add_library(${PROJECT_NAME} SHARED  ${sources})
target_include_directories(${PROJECT_NAME} PUBLIC ${includes})
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".xll")