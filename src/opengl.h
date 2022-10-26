#ifndef GLERROR_H
#define GLERROR_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glbinding/gl33core/gl.h>
#include <glbinding/Binding.h>

using namespace gl33core;

#include <iostream>

/** Rapporte les erreurs OpenGL, s'il y en a, sur stderr */
inline void _check_gl_error(const char *file, int line) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
        std::cout << "error: " << (int) error << " in " << file << " at l." << line << std::endl;
}

#define checkError() _check_gl_error(__FILE__,__LINE__)

#endif // GLERROR_H
