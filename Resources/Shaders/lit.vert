#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in float aTexIndex;

// In order for this shader to work with opengl and vulkan, we need to use a uniform buffer
// instead of a uniform. This is because vulkan does not allow us to use a uniform in a
// vertex shader. We can use a uniform buffer in both vertex and fragment shaders.
layout(binding = 0) uniform UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

// Vulkan requires that the location qualifier be used to specify the input variable index.
// OpenGL does not require this, but it is still good practice to use it.
layout(location = 0) out vec4 vColor;
layout(location = 1) out vec2 vTexCoord;
layout(location = 2) out float vTexIndex;

void main()
{
    vColor = aColor;
    vTexCoord = aTexCoord;
    vTexIndex = aTexIndex;
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(aPos, 1.0);
}