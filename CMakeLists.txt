cmake_minimum_required(VERSION 3.30)

project(AAD_book VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD              20)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS   ON)

option(ENABLE_WARNINGS "Enable compiler warnings for a target" ON)
option(ENABLE_WARNINGS_AS_ERRORS "Enable compiler warnings as errors" ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} -D_DEBUG")

if (${IS_DEBUG_BUILD})
    message(STATUS "We are in Debug")
else()
    message(STATUS "We are in Release")
endif ()

set(LIBRARY_NAME Library)
set(EXECUTABLE_NAME Executable)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

if(ENABLE_WARNINGS)
    include(Warnings)
endif()

add_subdirectory(aad_library)

# add_subdirectory(aad_work)
