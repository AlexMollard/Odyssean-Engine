// Vulkan fragment shader
#version 450
layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D texSampler;

void main() {
    // Light properties
    vec3 lightPos = vec3(0.0, -10.0, 1.0);
    vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
    float lightIntensity = 1.0;

    // Material properties
    vec4 diffuseColor = texture(texSampler, fragTexCoord);

    // Calculate lighting
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightColor.rgb * lightIntensity * diff * diffuseColor.rgb;
    vec3 ambient = lightColor.rgb * lightIntensity * 0.1 * diffuseColor.rgb;

    // Output color
    outColor = vec4(ambient + diffuse, 1.0);
}