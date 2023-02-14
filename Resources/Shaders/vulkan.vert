#version 450

layout(set = 0, binding = 0) uniform UBO {
mat4 modelViewProj;
} ubo;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 color;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec3 bitangent;

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;

void main() {
gl_Position = ubo.modelViewProj * vec4(position, 1.0);
fragPosition = position;
fragNormal = normal;
fragTexCoord = texCoord;
}