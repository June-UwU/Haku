#pragma once

#include <string>
#include <vector>
#include "types.hpp"
#include <GL/glew.h>

GLuint compileShader(std::string& filePath, GLenum shaderType);

GLuint createProgram(std::vector<GLuint>& shaders);

GLuint attachShader(GLuint program, GLuint shader);

void detactShaders(GLuint program, std::vector<GLuint>& shaders);

void deleteShaders(std::vector<GLuint>& shaders);

GLuint linkProgram(GLuint program);

