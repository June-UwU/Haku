#include "renderer.hpp"
#include "platform/platform.hpp"
#include <GL/glew.h>
#include "core/logger.hpp"
#include "shader_compiler.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>

static const GLfloat g_vertex_buffer_data[] = {
   -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
   0.0f,  1.0f, 0.0f,
};

static const GLfloat g_color_buffer_data[] = {
	0.583f,  0.771f,  0.014f,
    0.609f,  0.115f,  0.436f,
    0.327f,  0.483f,  0.844f
};

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

static const GLfloat g_cube_color_buffer_data[] = {
    0.583f,  0.771f,  0.014f,
    0.609f,  0.115f,  0.436f,
    0.327f,  0.483f,  0.844f,
    0.822f,  0.569f,  0.201f,
    0.435f,  0.602f,  0.223f,
    0.310f,  0.747f,  0.185f,
    0.597f,  0.770f,  0.761f,
    0.559f,  0.436f,  0.730f,
    0.359f,  0.583f,  0.152f,
    0.483f,  0.596f,  0.789f,
    0.559f,  0.861f,  0.639f,
    0.195f,  0.548f,  0.859f,
    0.014f,  0.184f,  0.576f,
    0.771f,  0.328f,  0.970f,
    0.406f,  0.615f,  0.116f,
    0.676f,  0.977f,  0.133f,
    0.971f,  0.572f,  0.833f,
    0.140f,  0.616f,  0.489f,
    0.997f,  0.513f,  0.064f,
    0.945f,  0.719f,  0.592f,
    0.543f,  0.021f,  0.978f,
    0.279f,  0.317f,  0.505f,
    0.167f,  0.620f,  0.077f,
    0.347f,  0.857f,  0.137f,
    0.055f,  0.953f,  0.042f,
    0.714f,  0.505f,  0.345f,
    0.783f,  0.290f,  0.734f,
    0.722f,  0.645f,  0.174f,
    0.302f,  0.455f,  0.848f,
    0.225f,  0.587f,  0.040f,
    0.517f,  0.713f,  0.338f,
    0.053f,  0.959f,  0.120f,
    0.393f,  0.621f,  0.362f,
    0.673f,  0.211f,  0.457f,
    0.820f,  0.883f,  0.371f,
    0.982f,  0.099f,  0.879f
};

GLuint vertexBuffer;
GLuint cubeVertexBuffer;
GLuint programID;
GLuint MatrixID;
GLuint cubeColorBuffer;
GLuint colorBuffer;
u32 rotateDegree = 0;

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
	
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexBuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &cubeVertexBuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_cube_buffer_data), g_vertex_cube_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &cubeColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_cube_color_buffer_data), g_cube_color_buffer_data, GL_STATIC_DRAW);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	programID = LoadShaders("/home/june/repos/Haku/shaders/simpleVertexShader.vertexshader",
							"/home/june/repos/Haku/shaders/simpleFragmentShader.fragmentshader");

	MatrixID = glGetUniformLocation(programID, "MVP");

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
	
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

	glUseProgram(programID);
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
	glBindBuffer(GL_ARRAY_BUFFER, cubeColorBuffer);
	glVertexAttribPointer(
	    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
	    3,                                // size
	    GL_FLOAT,                         // type
	    GL_FALSE,                         // normalized?
	    0,                                // stride
	    (void*)0                          // array buffer offset
	);
	
    // Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 12*3);

    glDisableVertexAttribArray(0);    
}

void clearScreen() {
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void shutdownRenderer() {

}

