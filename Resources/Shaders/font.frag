#version 450
#extension GL_KHR_vulkan_glsl : enable
layout(location = 0) in vec2 TexCoords;

layout(location = 0) out vec4 color;

// Texture samplers
layout (set = 0, binding = 1) uniform sampler2D text;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(sampled.rgb, sampled.a);
}