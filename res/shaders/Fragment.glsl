#version 450 core

in vec4 color;
in vec4 color_s;

out vec4 FragColor;

void main() {
    FragColor = color + color_s;
}