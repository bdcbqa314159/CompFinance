project(py_aad)

set(includes includes/ ../aad_ingestion/)
set(sources src/py_aad.cpp
            src/node.cpp
            # src/operations.cpp
            )


message(STATUS "[===] Python3_INCLUDE_DIRS: ${Python3_INCLUDE_DIRS}")
message(STATUS "[===] Python3_LIBRARIES: ${Python3_LIBRARIES}")
message(STATUS "[===] Python3_EXECUTABLE: ${Python3_EXECUTABLE}")

pybind11_add_module(py_aad ${sources}) 
target_include_directories(${PROJECT_NAME} PUBLIC ${includes} ${common_includes_dir} ${Python3_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC aad_ingestion)

# install(TARGETS py_aad
#   COMPONENT python
#   LIBRARY DESTINATION ${CUSTOM_LIBRARY_DIR_PYTHON}
#   )