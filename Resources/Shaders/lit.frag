#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec4 vColor;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in float vTexIndex;

layout(binding = 0) uniform sampler2D uTextures[32];

void main()
{
    FragColor = texture(uTextures[int(vTexIndex)], vTexCoord) * vColor;
}
