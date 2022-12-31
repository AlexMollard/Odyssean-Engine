// Vulkan fragment shader
#version 450
layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D texSampler;

void main() {
    vec3 lightPos = vec3(0.0, 0.0, 2.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 ambient = vec3(0.1, 0.1, 0.1);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(fragNormal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    // texSampler
    vec4 texColor = texture(texSampler, fragTexCoord);
    outColor = vec4((ambient + diffuse) * fragColor * texColor.rgb, texColor.a);
}