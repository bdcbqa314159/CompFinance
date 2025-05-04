project(aad_ingestion)

set(includes includes/)

set(sources src/AADNode.cpp
            src/AADTape.cpp
            src/analytics.cpp
#               src/mcBase.cpp
#               src/sobol.cpp
#               src/ThreadPool.cpp
)

add_library(${PROJECT_NAME} SHARED ${sources})
target_include_directories(${PROJECT_NAME} PUBLIC ${includes} ${common_includes_dir})
