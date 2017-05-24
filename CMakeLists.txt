
cmake_minimum_required(VERSION 3.0)

#get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#project(${PROJECT_NAME})
project(exercise2)

set(MATNR_AUTHOR1 "000000" CACHE STRING "")
set(MATNR_AUTHOR2 "000000" CACHE STRING "")

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
endif()


add_subdirectory(common)
add_subdirectory(e2task1)
add_subdirectory(e2task2)
add_subdirectory(e2task3)
add_subdirectory(e2task4)


# Package information

set(CPACK_PACKAGE_NAME        "${CMAKE_PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION     "${MATNR_AUTHOR1}-${MATNR_AUTHOR2}")
set(CPACK_PACKAGE_RELOCATABLE OFF)

# Package name
    
set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_SOURCE_INSTALLED_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/;/;")
set(CPACK_SOURCE_IGNORE_FILES "/build.*/;/\\\\.git/;\\\\.gitignore;\\\\.bat;\\\\.ppm;\\\\.txt\\\\.user;\\\\.raw;common/;main\\\\.cpp;CMakeLists\\\\.txt;/cmake/;.*\\\\.zip")

get_filename_component(CPACK_PATH ${CMAKE_COMMAND} PATH)
set(CPACK_COMMAND "${CPACK_PATH}/cpack")

add_custom_target(pack
    COMMAND ${CPACK_COMMAND} --config ${CMAKE_BINARY_DIR}/CPackSourceConfig.cmake
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

include(CPack)
