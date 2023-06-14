// FRAGMENT SHADER
#version 450

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

struct PointLight {
    vec3 color;
    vec3 position;
    float intensity;
} lights[4];

layout(std140, set = 1, binding = 0) uniform PointLights {
    PointLight pointLights;
};

layout(set = 2, binding = 0) uniform sampler2D diffuseMap;
layout(set = 2, binding = 1) uniform sampler2D normalMap;
layout(set = 2, binding = 2) uniform sampler2D metallicMap;
layout(set = 2, binding = 3) uniform sampler2D roughnessMap;
layout(set = 2, binding = 4) uniform sampler2D ambientMap;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 normal = texture(normalMap, fragTexCoord).rgb;
    normal = normalize(2.0 * normal - 1.0);
    vec3 N = normalize(mix(normal, fragNormal, 0.5));

    vec3 V = -fragPosition.xyz;
    vec3 diffuseColor = texture(diffuseMap, fragTexCoord).rgb;
    vec3 ambientColor = texture(ambientMap, fragTexCoord).rgb;
    float roughness = texture(roughnessMap, fragTexCoord).r;
    float metallic = texture(metallicMap, fragTexCoord).r;

    outColor = vec4(diffuseColor, 1.0);
}
