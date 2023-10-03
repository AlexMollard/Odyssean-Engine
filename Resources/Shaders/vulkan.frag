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

layout(set = 2, binding = 0) uniform sampler2D diffuseMap;
layout(set = 2, binding = 1) uniform sampler2D normalMap;
layout(set = 2, binding = 2) uniform sampler2D metallicMap;
layout(set = 2, binding = 3) uniform sampler2D roughnessMap;
layout(set = 2, binding = 4) uniform sampler2D ambientMap;

layout(location = 0) out vec4 outColor;

vec3 calculateColor(vec3 diffuseColor, vec3 ambientColor, vec3 normal, vec3 viewDirection, float roughness, float metallic) {
    vec3 lightColor = vec3(0.0);

    for (int i = 0; i < 4; i++) {
        // Calculate the direction from the fragment to the light source
        vec3 lightDirection = normalize(lights[i].position - fragPosition);

        // Calculate the diffuse reflection (Lambertian reflectance)
        float diffuseFactor = max(dot(normal, lightDirection), 0.0);

        // Calculate the specular reflection (Phong reflection)
        vec3 halfway = normalize(lightDirection + viewDirection);
        float specularFactor = pow(max(dot(normal, halfway), 0.0), 32.0);

        // Combine diffuse and specular reflections with light color and intensity
        vec3 lightContribution = lights[i].color * lights[i].intensity * (
            diffuseFactor * (1.0 - metallic) + specularFactor * metallic
        );

        // Add the contribution of this light source to the final color
        lightColor += lightContribution;
    }

    // Combine ambient and diffuse lighting with material color
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
    float metallic = texture(metallicMap, fragTexCoord).r;

    vec3 finalColor = calculateColor(diffuseColor, ambientColor, N, V, roughness, metallic);

    outColor = vec4(finalColor, 1.0);
}
