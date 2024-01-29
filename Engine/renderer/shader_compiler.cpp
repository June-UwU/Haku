#include "shader_compiler.hpp"
#include <fstream>
#include <sstream>
#include "core/logger.hpp"

GLuint compileShader(std::string& filePath, GLenum shaderType) {
    
    std::string shaderCode;
	std::ifstream shaderStream(filePath, std::ios::in);
	if(true == shaderStream.is_open()){
		std::stringstream sstr;
		sstr << shaderStream.rdbuf();
		shaderCode = sstr.str();
		shaderStream.close();
	}else{
		LOG_CRITICAL("Impossible to open %s. Are you in the right directory ?!\n", filePath.c_str());
		return 0;
	}

	GLuint shader = glCreateShader(shaderType);

	LOG_TRACE("Compiling shader");
	char const * source = shaderCode.c_str();
	glShaderSource(shader, 1, &source , NULL);
	glCompileShader(shader);

    GLint Result = GL_FALSE;
	int infoLogLength;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(infoLogLength+1);
		glGetShaderInfoLog(shader, infoLogLength, NULL, &VertexShaderErrorMessage[0]);
		LOG_WARN("%s\n", &VertexShaderErrorMessage[0]);
	}

    return shader;
}

GLuint createProgram(std::vector<GLuint>& shaders) {
    GLuint program = glCreateProgram();

    for(auto& shader: shaders) {
        glAttachShader(program, shader);
    }

    glLinkProgram(program);

    GLint result = GL_FALSE;
    s32 infoLogLength = 0;

    glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(infoLogLength+1);
		glGetProgramInfoLog(program, infoLogLength, NULL, &ProgramErrorMessage[0]);
		LOG_CRITICAL("%s\n", &ProgramErrorMessage[0]);
        return 0;
	}

    return program;
}

GLuint attachShader(GLuint program, GLuint shader) {
    glAttachShader(program, shader);
    return program;
}

void detactShaders(GLuint program, std::vector<GLuint>& shaders) {
    for(auto& shader: shaders) {
        glDetachShader(program, shader);
    }
}

void deleteShaders(std::vector<GLuint>& shaders) {
    for(auto& shader: shaders) {
        glDeleteShader(shader);
    }
}

GLuint linkProgram(GLuint program) {
    glLinkProgram(program);

    GLint Result = GL_FALSE;
    s32 infoLogLength = 0;

	glGetProgramiv(program, GL_LINK_STATUS, &Result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(infoLogLength+1);
		glGetProgramInfoLog(program, infoLogLength, NULL, &ProgramErrorMessage[0]);
		LOG_CRITICAL("%s\n", &ProgramErrorMessage[0]);
        return 0;
	}

    return program;
}
