#include "renderer.hpp"
#include "platform/platform.hpp"
#include <GL/glew.h>
#include "core/logger.hpp"
#include "shader_compiler.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include <gli/gli.hpp>
#include <glm/gtc/matrix_transform.hpp>

static const GLfloat g_vertex_cube_buffer_data[] = {
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

static const GLfloat g_uv_buffer_data[] = {
    0.000059f, 1.0f-0.000004f,
    0.000103f, 1.0f-0.336048f,
    0.335973f, 1.0f-0.335903f,
    1.000023f, 1.0f-0.000013f,
    0.667979f, 1.0f-0.335851f,
    0.999958f, 1.0f-0.336064f,
    0.667979f, 1.0f-0.335851f,
    0.336024f, 1.0f-0.671877f,
    0.667969f, 1.0f-0.671889f,
    1.000023f, 1.0f-0.000013f,
    0.668104f, 1.0f-0.000013f,
    0.667979f, 1.0f-0.335851f,
    0.000059f, 1.0f-0.000004f,
    0.335973f, 1.0f-0.335903f,
    0.336098f, 1.0f-0.000071f,
    0.667979f, 1.0f-0.335851f,
    0.335973f, 1.0f-0.335903f,
    0.336024f, 1.0f-0.671877f,
    1.000004f, 1.0f-0.671847f,
    0.999958f, 1.0f-0.336064f,
    0.667979f, 1.0f-0.335851f,
    0.668104f, 1.0f-0.000013f,
    0.335973f, 1.0f-0.335903f,
    0.667979f, 1.0f-0.335851f,
    0.335973f, 1.0f-0.335903f,
    0.668104f, 1.0f-0.000013f,
    0.336098f, 1.0f-0.000071f,
    0.000103f, 1.0f-0.336048f,
    0.000004f, 1.0f-0.671870f,
    0.336024f, 1.0f-0.671877f,
    0.000103f, 1.0f-0.336048f,
    0.336024f, 1.0f-0.671877f,
    0.335973f, 1.0f-0.335903f,
    0.667969f, 1.0f-0.671889f,
    1.000004f, 1.0f-0.671847f,
    0.667979f, 1.0f-0.335851f
};

GLuint cubeVertexBuffer;
GLuint programID;
GLuint MatrixID;
GLuint cubeTexture;
GLuint cubeUV;
GLuint samplerID;
u32 rotateDegree = 0;

GLuint createTexture(std::string filePath) {
	gli::texture texture = gli::load(filePath);
	if(texture.empty())
		return 0;

	gli::gl GL(gli::gl::PROFILE_GL33);
	gli::gl::format const format = GL.translate(texture.format(), texture.swizzles());
	GLenum target = GL.translate(texture.target());

	GLuint textureName = 0;
	glGenTextures(1, &textureName);
	glBindTexture(target, textureName);
	glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(texture.levels() - 1));
	glTexParameteriv(target, GL_TEXTURE_SWIZZLE_RGBA, &format.Swizzles[0]);
	
	glm::tvec3<GLsizei> Extent(texture.extent(0));
	glTexStorage2D(target, static_cast<GLint>(texture.levels()), format.Internal, Extent.x, Extent.y);

	for(std::size_t Level = 0; Level < texture.levels(); ++Level)
	{
		glm::tvec3<GLsizei> Extent(texture.extent(Level));
		glCompressedTexSubImage2D(
			target, static_cast<GLint>(Level), 0, 0, Extent.x, Extent.y,
			format.Internal, static_cast<GLsizei>(texture.size(Level)), texture.data(0, 0, Level));
	}

	return textureName;
}

GLuint LoadShaders(std::string vertexFilePath, std::string fragmentFilePath) {
	GLuint vertexID = compileShader(vertexFilePath, GL_VERTEX_SHADER);
	GLuint fragmentID = compileShader(fragmentFilePath, GL_FRAGMENT_SHADER);

	std::vector<GLuint> shaders = {vertexID, fragmentID};

	GLuint program = createProgram(shaders);

	detactShaders(program, shaders);
	deleteShaders(shaders);

	return program;
}

[[nodiscard]] Status initializeRenderer() {	
    GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

    glGenBuffers(1, &cubeVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_cube_buffer_data), g_vertex_cube_buffer_data, GL_STATIC_DRAW);
	
	glGenBuffers(1, &cubeUV);
	glBindBuffer(GL_ARRAY_BUFFER, cubeUV);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	programID = LoadShaders("/home/june/repos/Haku/shaders/simpleVertexShader.vertexshader",
							"/home/june/repos/Haku/shaders/simpleFragmentShader.fragmentshader");

    cubeTexture = createTexture("/home/june/Downloads/uvtemplate.DDS");

	MatrixID = glGetUniformLocation(programID, "MVP");

    samplerID = glGetUniformLocation(programID, "myTextureSampler");

    return OK;
}

void render() {
	u32 width = getWindowWidth();
	u32 height = getWindowHeight();

	rotateDegree++;
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);
	
	glm::mat4 View = glm::lookAt(
    	glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
    	glm::vec3(0,0,0), // and looks at the origin
    	glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
    	);

	// Model matrix : an identity matrix (model will be at the origin)

	glm::mat4 Model = glm::rotate(glm::mat4(1.0f), glm::radians((float)rotateDegree), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::mat4(1.0f);;

	glm::mat4 mvp = Projection * View * Model;
    
    glUseProgram(programID);

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubeTexture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(samplerID, 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
    glVertexAttribPointer(
       0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
       3,                  // size
       GL_FLOAT,           // type
       GL_FALSE,           // normalized?
       0,                  // stride
       (void*)0            // array buffer offset
    );

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, cubeUV);
	glVertexAttribPointer(
	    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
	    2,                                // size
	    GL_FLOAT,                         // type
	    GL_FALSE,                         // normalized?
	    0,                                // stride
	    (void*)0                          // array buffer offset
	);
	
	glDrawArrays(GL_TRIANGLES, 0, 12*3);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);    
}

void clearScreen() {
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void shutdownRenderer() {

}

