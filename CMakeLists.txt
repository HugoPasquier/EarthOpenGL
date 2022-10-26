project(EarthOpenGL)

cmake_minimum_required(VERSION 3.2.0)

add_subdirectory(ext/glfw)
add_subdirectory(ext/glbinding)
add_subdirectory(ext/SOIL)

include_directories(ext/glfw/include)
include_directories(ext/glbinding/include)
include_directories(ext/eigen3)
include_directories(ext/SOIL/src)
include_directories(ext/)

set (CMAKE_CXX_STANDARD 11)

if(APPLE)
    add_definitions(-DGL_SILENCE_DEPRECATION)
endif()

set(SRC_FILES
    ext/ObjFormat/ObjFormat.cpp
    ext/ObjFormat/ObjUtil.cpp
    src/main.cpp
    src/viewer.cpp
    src/viewer.h
    src/shader.cpp
    src/shader.h
    src/opengl.h
    src/camera.h
    src/camera.cpp
    src/mesh.h
    src/mesh.cpp
    src/trackball.cpp
    src/trackball.h
)

add_definitions(-DDATA_DIR="${PROJECT_SOURCE_DIR}/data")

add_executable(EartchOpenGL_viewer ${SRC_FILES})

target_link_libraries(EartchOpenGL_viewer glfw ${GLFW_LIBRARIES} glbinding SOIL)

function(IndicateExternalFile _target)
    foreach(_file IN ITEMS ${ARGN})
        if ((IS_ABSOLUTE "${_file}" AND EXISTS "${_file}") OR
            (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_file}"))
            target_sources(${_target} PRIVATE "${_file}")
        endif()
    endforeach()
endfunction()

file(GLOB SHADERS "data/shaders/*.vert" "data/shaders/*.frag")
IndicateExternalFile("EartchOpenGL_viewer" ${SHADERS})
