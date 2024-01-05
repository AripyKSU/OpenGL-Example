#version 330 core

//in vec4 vertexColor;
uniform vec4 color;
out vec4 fragColor;

void main() {
    fragColor = color;
}