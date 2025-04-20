project(aad)

set(includes includes/)
set(sources src/aad_library.cpp
            src/AAD.cpp
            src/mcBase.cpp
            src/sobol.cpp
            src/ThreadPool.cpp
            # src/xlcall.cpp
            # src/xlExport.cpp
            # src/xlframework.c
            # src/xlMemoryManager.cpp
            # src/xlMemoryPool.cpp

)

# assuming at the moment that we are only msvc

# add_compile_definitions(${PROJECT_NAME}_EXPORTS
#                         _WINDLL
# )

# add_library(${PROJECT_NAME} SHARED  ${sources})
# target_include_directories(${PROJECT_NAME} PUBLIC ${includes})
# set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".xll")