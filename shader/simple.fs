#version 330 core

//in vec4 vertexColor;
out vec4 fragColor;

uniform vec4 color;

void main() {
    fragColor = color;
}