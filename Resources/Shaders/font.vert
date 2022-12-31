#version 450
// This font shader will be used in opengl and vulkan
// spir-v requires a location for every input and output
#extension GL_KHR_vulkan_glsl : enable
layout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>

layout(location = 0) out vec2 TexCoords;

out gl_PerVertex {
    vec4 gl_Position;
};

// Vulkan uniforms are done differently than OpenGL
layout (set = 0, binding = 0)
uniform UniformBufferObject {
    mat4 projection;
} ubo;

void main()
{
    gl_Position = ubo.projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}