#version 450 core

uniform mat4 mvp;
layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;
out vec4 color;

void main() {
    gl_Position = mvp * inPosition;
    color = inColor;
}