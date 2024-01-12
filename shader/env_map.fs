#version 330 core

out vec4 fragColor;

in vec3 normal;
in vec3 position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main() {
    //I: eye, R: Reflect
    vec3 I = normalize(position - cameraPos);
    vec3 R = reflect(I, normalize(normal));
    //큐브에 R ray를 쏴서 그 색상을 가져옴
    fragColor = vec4(texture(skybox, R).rgb, 1.0);
}