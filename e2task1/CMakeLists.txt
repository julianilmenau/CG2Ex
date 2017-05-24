
set(target e2task1)

find_package(GLFW REQUIRED)
find_package(glbinding REQUIRED)
find_package(GLM REQUIRED)

set(sources
    main.cpp
    
    e2task1.h
    e2task1.cpp
    e2task1.vert
    e2task1.frag
)

add_executable(${target} ${sources})

target_link_libraries(${target} common)
target_include_directories(${target} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
