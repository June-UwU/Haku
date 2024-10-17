#version 450

layout(push_constant) uniform MVP {
    mat4 projection;
    mat4 view;
    mat4 model;
} mvp;

layout(location = 0) in vec2 positions;
layout(location = 1) in vec3 colors;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = vec4(positions, 0.0, 1.0);
    fragColor = colors;
}