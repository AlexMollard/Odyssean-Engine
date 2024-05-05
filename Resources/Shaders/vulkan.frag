#version 450
#extension GL_KHR_vulkan_glsl : enable // Enable the Vulkan GLSL extension

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

struct PointLight {
    vec3 color;
    vec3 position;
    float intensity;
};

layout(set = 1, binding = 0) buffer PointLights {
    PointLight lights[4];
};

// Bindings for textures
layout(set = 2, binding = 0) uniform sampler2D diffuseMap;
layout(set = 2, binding = 1) uniform sampler2D specularMap;
layout(set = 2, binding = 2) uniform sampler2D normalMap;
layout(set = 2, binding = 3) uniform sampler2D heightMap;
layout(set = 2, binding = 4) uniform sampler2D ambientMap;
layout(set = 2, binding = 5) uniform sampler2D emissiveMap;
layout(set = 2, binding = 6) uniform sampler2D shininessMap;
layout(set = 2, binding = 7) uniform sampler2D opacityMap;
layout(set = 2, binding = 8) uniform sampler2D displacementMap;
layout(set = 2, binding = 9) uniform sampler2D lightMap;
layout(set = 2, binding = 10) uniform sampler2D reflectionMap;
layout(set = 2, binding = 11) uniform sampler2D metallicMap;
layout(set = 2, binding = 12) uniform sampler2D roughnessMap;

layout(location = 0) out vec4 outColor;

vec3 calculateColor(vec3 diffuseColor, vec3 ambientColor, vec3 normal, vec3 viewDirection, float roughness, float metallic) {
    vec3 lightColor = vec3(1.0); // Initialize light color to zero

    for (int i = 0; i < 4; i++) {
        if (lights[i].intensity > 0.0) { // Check if light intensity is greater than zero
            vec3 lightDirection = normalize(lights[i].position - fragPosition);
            float diffuseFactor = max(dot(normal, lightDirection), 0.0);
            vec3 halfway = normalize(lightDirection + viewDirection);
            float specularFactor = pow(max(dot(normal, halfway), 0.0), 32.0);
            vec3 lightContribution = lights[i].color * lights[i].intensity * (
                diffuseFactor * (1.0 - metallic) + specularFactor * metallic
            );
            lightColor += lightContribution;
        }
    }

    vec3 finalColor = (ambientColor + diffuseColor * lightColor) * (1.0 - metallic);
    return finalColor;
}

void main() {
    vec3 normal = texture(normalMap, fragTexCoord).rgb;
    normal = normalize(2.0 * normal - 1.0);
    vec3 N = normalize(mix(normal, fragNormal, 0.5));

    vec3 V = -fragPosition.xyz;
    vec3 diffuseColor = texture(diffuseMap, fragTexCoord).rgb;
    vec3 ambientColor = texture(ambientMap, fragTexCoord).rgb * 0.5;
    float roughness = texture(roughnessMap, fragTexCoord).r;
    float metallic = texture(metallicMap, fragTexCoord).r; // Correctly sample metallicMap

    vec3 finalColor = calculateColor(diffuseColor, ambientColor, N, V, roughness, metallic);

    outColor = vec4(finalColor, 1.0);
}
