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

mat4 lookAt(vec3 eye, vec3 center, vec3 up) {
    vec3 f = normalize(center - eye);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);

    mat4 view = mat4(1.0);
    view[0][0] = s.x;
    view[1][0] = s.y;
    view[2][0] = s.z;
    view[0][1] = u.x;
    view[1][1] = u.y;
    view[2][1] = u.z;
    view[0][2] = -f.x;
    view[1][2] = -f.y;
    view[2][2] = -f.z;
    view[3][0] = -dot(s, eye);
    view[3][1] = -dot(u, eye);
    view[3][2] = dot(f, eye);
    return view;
}

mat4 perspective(float fovy, float aspect, float zNear, float zFar) {
    float tanHalfFovy = tan(fovy / 2.0);

    mat4 proj = mat4(0.0);
    proj[0][0] = 1.0 / (aspect * tanHalfFovy);
    proj[1][1] = 1.0 / (tanHalfFovy);
    proj[2][2] = -(zFar + zNear) / (zFar - zNear);
    proj[2][3] = -1.0;
    proj[3][2] = -(2.0 * zFar * zNear) / (zFar - zNear);
    return proj;
}

mat4 moveCamera(mat4 view, float x, float y, float z) {
    view[3][0] = x;
    view[3][1] = y;
    view[3][2] = z;
    return view;
}

void main() {
    mat4 newView = mat4(1.0);
    mat4 newProj = mat4(1.0);

    // Set the new matrices to the uniform
    if (ubo.view == mat4(0.0))
    {
        newView = lookAt(vec3(0.0, 0.0, 3.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
        newView = moveCamera(newView, 0.0, 0.0, -1.0);
        newProj = perspective(radians(60.0), -1920.0 / -1080.0, 0.1, 100.0);
    } else {
        newView = ubo.view;
        newProj = ubo.proj;
    }

    fragPos = (ubo.model * vec4(position, 1.0)).xyz;
    fragNormal = (ubo.model * vec4(normal, 0.0)).xyz;
    fragColor = color;
    fragTexCoord = texCoord;
    gl_Position = newProj * newView * ubo.model * vec4(position, 1.0);
}