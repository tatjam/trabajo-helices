cmake_minimum_required (VERSION 3.2.3)
set (CMAKE_CXX_STANDARD 17)

project(helices)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(helices ${SOURCES})

if(MSVC)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
else()
    target_compile_options(helices PUBLIC -g -O0)
endif()

include_directories(dep)
target_link_libraries(helices)
