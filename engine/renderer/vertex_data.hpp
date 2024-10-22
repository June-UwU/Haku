#pragma once
#include "glm/glm.hpp"

typedef struct vertex {
	glm::vec2 pos;
	glm::vec3 col;
} vertex;

typedef struct mvp {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
} mvp;