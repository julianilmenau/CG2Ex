
set(target e2task4)

find_package(OpenMP QUIET)
if (NOT OPENMP_FOUND)
    message("Loop parallelization in ${target} skipped: OpenMP not found")
endif()

find_package(GLFW REQUIRED)
find_package(glbinding REQUIRED)
find_package(GLM REQUIRED)

set(sources
    main.cpp
    
    e2task4.h
    e2task4.cpp
    e2task4.vert
    e2task4.frag
)

add_executable(${target} ${sources})

target_compile_options(${target}
    PUBLIC
    $<$<BOOL:"${OPENMP_FOUND}">:${OpenMP_CXX_FLAGS}>
)

target_link_libraries(${target}
    PRIVATE
    common
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:$<$<BOOL:"${OPENMP_FOUND}">:${OpenMP_CXX_FLAGS}>>
)

target_include_directories(${target} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
