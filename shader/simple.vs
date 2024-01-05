#version 330 core
//VAO attribute 0번, 점이 하나씩 aPos에 들어옴
layout (location = 0) in vec3 aPos;

//out vec4 vertexColor; //fragment shader로 넘어갈 컬러값
uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(aPos, 1.0); //vertex shader 내용
    //vertexColor = vec4(0.5, 0.5, 0.0, 1.0);  // rasterization 해서 out으로 fs에 넘김
}