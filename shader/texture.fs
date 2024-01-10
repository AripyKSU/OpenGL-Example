#version 330 core
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;

void main() {
    vec4 pixel = texture(tex, texCoord);
    //blend 예제에서 창 모서리에 투명한 부분을 discard해서 역방향에서 볼 때 모서리 depth test 문제 해결
    if (pixel.a < 0.01)
        discard;
    fragColor = pixel;
}