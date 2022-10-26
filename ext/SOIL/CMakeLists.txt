cmake_minimum_required (VERSION 2.6)
project (SOIL)

if(ANDROID)
  # Android requires GL ES 2.0 package automatically
  set(LIBRARIES GLESv2)
else()
  find_package (OpenGL REQUIRED)
  include_directories (${OPENGL_INCLUDE_DIR})
  set(LIBRARIES ${OPENGL_LIBRARIES})
endif()

if(WIN32)
	add_definitions (-D_CRT_SECURE_NO_WARNINGS)
endif()

include_directories(src)

add_library (SOIL STATIC src/image_helper.c src/image_DXT.c src/SOIL2.c src/etc1_utils.c)

target_link_libraries (SOIL ${LIBRARIES})