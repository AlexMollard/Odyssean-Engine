// Vulkan fragment shader
#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;
layout(set = 0, binding = 2) uniform LightProperties {
    vec3 lightPos;
    vec4 lightColor;
} light;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    // Normalize inputs
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(light.lightPos - fragPos);

    // Calculate diffuse lighting
    float diffuseFactor = max(dot(norm, lightDir), 0.0);
    vec4 diffuseColor = texture(texSampler, fragTexCoord);
    vec3 diffuse = light.lightColor.rgb * light.lightColor.a * diffuseFactor * diffuseColor.rgb;

    // Calculate ambient lighting
    vec3 ambient = light.lightColor.a * 0.1 * diffuseColor.rgb;

    // Output color
    outColor = vec4(ambient + diffuse, 1.0);
}