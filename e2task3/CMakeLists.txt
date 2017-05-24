
set(target e2task3)

find_package(GLFW REQUIRED)
find_package(glbinding REQUIRED)
find_package(GLM REQUIRED)

set(sources
    main.cpp
    
    e2task3.h
    e2task3.cpp
    e2task3.vert
    e2task3.geom
    e2task3.frag
)

add_executable(${target} ${sources})

target_link_libraries(${target} common)
target_include_directories(${target} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
