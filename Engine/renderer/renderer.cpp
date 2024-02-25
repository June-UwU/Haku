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

GLuint samplerID;
u32 rotateDegree = 0;

GLuint createTexture(std::string filePath) {
	gli::texture texture = gli::load(filePath);
	if(texture.empty()) {
        LOG_CRITICAL("Empty texture returned..!");
		return 0;
    }

	gli::gl GL(gli::gl::PROFILE_GL33);
	gli::gl::format const format = GL.translate(texture.format(), texture.swizzles());
	GLenum target = GL.translate(texture.target());

	GLuint textureName = 0;
	glGenTextures(1, &textureName);
	glBindTexture(target, textureName);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);
	
	glm::tvec3<GLsizei> Extent(texture.extent(0));
	glTexStorage2D(target, static_cast<GLint>(texture.levels()), format.Internal, Extent.x, Extent.y);

	for(std::size_t Level = 0; Level < texture.levels(); ++Level) {
		glm::tvec3<GLsizei> Extent(texture.extent(Level));
		glCompressedTexSubImage2D(
			target, static_cast<GLint>(Level), 0, 0, Extent.x, Extent.y,
			format.Internal, static_cast<GLsizei>(texture.size(Level)), texture.data(0, 0, Level));
	}

    LOG_TRACE("Texture load complete!");
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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);

	programID = LoadShaders("/home/june/repos/Haku/shaders/simpleVertexShader.vertexshader",
							"/home/june/repos/Haku/shaders/simpleFragmentShader.fragmentshader");

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

	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(samplerID, 0);
}

void clearScreen() {
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void shutdownRenderer() {

}

