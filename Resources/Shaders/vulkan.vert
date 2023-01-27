#version 450

layout(set = 0, binding = 0) uniform UBO {
mat4 modelViewProj;
} ubo;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;

void main() {
gl_Position = ubo.modelViewProj * vec4(position, 1.0);
fragNormal = normal;
fragTexCoord = texCoord;
}