#version 330 core
in vec3 normal;
in vec2 texCoord;
in vec3 position;
out vec4 fragColor;

uniform vec3 viewPos;

struct Light {
    vec3 position;
    vec3 direction;
    //spotlight의 각도 최대값(cos)
    vec2 cutoff;
    vec3 attenuation;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
//blinn shading 스위치
uniform int blinn;
 
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

void main() {
    vec3 texColor = texture2D(material.diffuse, texCoord).xyz;
    //ambient
    vec3 ambient = texColor * light.ambient;

    //attenuation 계산
    float dist = length(light.position - position);
    vec3 distPoly = vec3(1.0, dist, dist*dist);
    float attenuation = 1.0 / dot(distPoly, light.attenuation);

    //diffuse, specular, spotlight에서의 계산
    vec3 lightDir = (light.position - position) / dist;
    
    vec3 result = ambient;
    float theta = dot(lightDir, normalize(-light.direction));
    //내부각과 외부각 사이에서 clamp(연속된 값)
    float intensity = clamp(
        (theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]), 
        0.0, 1.0);

    if (intensity > 0.0) {
        vec3 pixelNorm = normalize(normal);
        float diff = max(dot(pixelNorm, lightDir), 0.0);
        vec3 diffuse = diff * texColor * light.diffuse;

        //blinn-phong specular 계산
        vec3 specColor = texture2D(material.specular, texCoord).xyz;
        float spec = 0.0;
        if (blinn == 0) {
            vec3 viewDir = normalize(viewPos - position);
            vec3 reflectDir = reflect(-lightDir, pixelNorm);
            spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        }
        else {
            vec3 viewDir = normalize(viewPos - position);
            vec3 halfDir = normalize(lightDir + viewDir);
            spec = pow(max(dot(halfDir, pixelNorm), 0.0), material.shininess);
        }
        vec3 specular = spec * specColor * light.specular;

        result += (diffuse + specular) * intensity;
    }
    result *= attenuation;

    //fragColor = vec4(vec3(gl_FragCoord.z), 1.0);
    fragColor = vec4(result, 1.0);
}