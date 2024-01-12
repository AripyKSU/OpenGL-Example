#version 330 core
out vec4 fragColor;
in vec3 texCoord;

//opengl의 samplerCube 이용
uniform samplerCube skybox;

void main() {
    fragColor = texture(skybox, texCoord);
}