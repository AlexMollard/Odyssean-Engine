// vulkan fragment shader
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

// Vertex shader outputs texcoord and color
layout(location = 0) in vec3 color;
layout(location = 1) in vec2 texcoord;

// Fragment shader outputs a color
layout(location = 0) out vec4 outColor;

// Texture sampler
//layout(binding = 0) uniform sampler2D u_sampler;

void main()
{

    // Output color = color of the texture + color of the vertex
    outColor = vec4(color, 1.0);
}