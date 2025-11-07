#ifndef _SHADERS_H_
#define _SHADERS_H_

// Simple shaders (GLSL 410 core)
static const char* vertexShaderSource = R"GLSL(
#version 410 core
layout (location = 0) in vec2 aPos;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)GLSL";

static const char* fragmentShaderSource = R"GLSL(
#version 410 core
out vec4 FragColor;
void main() {
    FragColor = vec4(0.85, 0.2, 0.35, 1.0);
}
)GLSL";

#endif