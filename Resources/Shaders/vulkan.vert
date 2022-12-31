// Vulkan vertex shader
#version 450
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 color;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragColor;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    fragPos = (ubo.model * vec4(position, 1.0)).xyz;
    fragNormal = (ubo.model * vec4(normal, 0.0)).xyz;
    fragColor = color;
    fragTexCoord = texCoord;
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(position, 1.0);
}