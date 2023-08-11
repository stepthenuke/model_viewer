#ifndef SHADER_H
#define SHADER_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/glad/glad.h"
#include <GLFW/glfw3.h>

char *read_shader(const char *path_to_shader);
GLuint compile_shader(const char *shader_source, GLenum shader_type);
GLuint load_program(const char *vert_shader_source, const char *tess_shader_source, const char *frag_shader_source);

#endif //SHADER_H
