
set(target common)

find_package(GLFW REQUIRED)
find_package(glbinding REQUIRED)
find_package(GLM REQUIRED)

set(sources
    common.h
    common.cpp
    Cube.h
    Cube.cpp
    Cylinder.h
    Cylinder.cpp
    Icosahedron.h
    Icosahedron.cpp
    Plane.h
    Plane.cpp
)

add_library(${target} STATIC ${sources})

target_link_libraries(${target} PUBLIC ${GLFW_LIBRARIES} glbinding::glbinding)
target_include_directories(${target} PUBLIC ${GLFW_INCLUDE_DIR} ${GLM_INCLUDE_DIR} $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_compile_definitions(${target} PUBLIC GLFW_INCLUDE_NONE GLM_FORCE_RADIANS)
