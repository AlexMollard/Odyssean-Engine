#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;
layout(set = 0, binding = 2) uniform LightProperties {
    vec4 lightColor;
    vec3 lightPos;
} light;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragColor;
layout(location = 4) in float roughness;

layout(location = 0) out vec4 outColor;

const float PI = 3.14159265358979323846;
const float epsilon = 0.0001;

// Function to calculate the Half-vector
vec3 HalfVector(vec3 lightDirection, vec3 viewDirection) {
    return normalize(lightDirection + viewDirection);
}

// Function to calculate the diffuse term
float DiffuseTerm(vec3 lightDirection, vec3 surfaceNormal) {
    return max(dot(lightDirection, surfaceNormal), 0.0);
}

// Function to calculate the specular term
float SpecularTerm(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float roughness) {
    vec3 halfVector = HalfVector(lightDirection, viewDirection);
    float dotProduct = dot(surfaceNormal, halfVector);
    float roughnessSquared = roughness * roughness;
    float specular = pow(max(dotProduct, 0.0), 1.0 / roughnessSquared);
    return specular;
}

// Function to calculate the geometric term
float GeometricTerm(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float roughness) {
    vec3 halfVector = HalfVector(lightDirection, viewDirection);
    float dotProductView = dot(viewDirection, surfaceNormal);
    float dotProductLight = dot(lightDirection, surfaceNormal);
    float dotProductHalf = dot(halfVector, surfaceNormal);
    float geometric = min(min(2.0 * dotProductHalf * dotProductView / dotProductLight, 2.0 * dotProductHalf * dotProductLight / dotProductView), 1.0);
    return geometric;
}

// Function to calculate the fresnel term
float FresnelTerm(vec3 viewDirection, vec3 surfaceNormal, float roughness) {
    float fresnel = pow(1.0 - max(dot(viewDirection, surfaceNormal), 0.0), 5.0);
    return fresnel;
}

void main() {
    // Normalize inputs
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(light.lightPos - fragPos);
    vec3 viewDirection = normalize(-fragPos);

    // Calculate diffuse term
    float diffuse = DiffuseTerm(lightDir,norm);

    // Calculate specular term
    float specular = SpecularTerm(lightDir, viewDirection, norm, roughness);

    // Calculate geometric term
    float geometric = GeometricTerm(lightDir, viewDirection, norm, roughness);

    // Calculate fresnel term
    float fresnel = FresnelTerm(viewDirection, norm, roughness);

    // Calculate diffuse color
    vec4 diffuseColor = texture(texSampler, fragTexCoord);

    // Calculate final color
    vec3 finalColor = light.lightColor.rgb * (diffuse * diffuseColor.rgb + specular * fresnel * geometric);

    vec3 ambientColor = diffuseColor.rgb * 0.1;

    // Output color
    outColor = vec4(finalColor + ambientColor, 1.0);

}