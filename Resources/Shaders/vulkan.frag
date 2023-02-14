// FRAGMENT SHADER
#version 450

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

struct PointLight {
    vec3 color;
    vec3 _padding0[4];
    vec3 position;
    vec3 _padding1[4];
    float intensity;
    vec3 _padding2[4];
};

struct DirectionalLight
{
	vec3 color;
	vec3 direction;
	float intensity;
};

struct SpotLight
{
	vec3 color;
	vec3 position;
	vec3 direction;
	float intensity;
	float cutoff;
};


layout(std140, set = 1, binding = 0) uniform PointLights {
    PointLight lights[4];
} pointLights;

layout(set = 1, binding = 1) uniform DirectionalLights {
    DirectionalLight lights[4];
} directionalLights;

layout(set = 1, binding = 2) uniform SpotLights {
    SpotLight lights[4];
} spotLights;

layout(set = 2, binding = 0) uniform sampler2D diffuseMap;
layout(set = 2, binding = 1) uniform sampler2D normalMap;
layout(set = 2, binding = 2) uniform sampler2D metallicMap;
layout(set = 2, binding = 3) uniform sampler2D roughnessMap;
layout(set = 2, binding = 4) uniform sampler2D ambientMap;

layout(location = 0) out vec4 outColor;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

void main() {
    vec3 normal = texture(normalMap, fragTexCoord).rgb;
    normal = normalize(2.0 * normal - 1.0);
    vec3 N = normalize(mix(normal, fragNormal, 0.5));

    vec3 V = normalize(-fragPosition.xyz);
    vec3 diffuseColor = texture(diffuseMap, fragTexCoord).rgb;
    vec3 ambientColor = texture(ambientMap, fragTexCoord).rgb;
    float roughness = texture(roughnessMap, fragTexCoord).r;
    float metallic = texture(metallicMap, fragTexCoord).r;

    vec3 totalLight = vec3(0.0);
    vec3 F0 = mix(vec3(0.04), diffuseColor, metallic);

    for(int i = 0; i < 1; i++) {
        PointLight light = pointLights.lights[i];

        vec3 lightDirection = normalize(light.position - fragPosition.xyz);
        vec3 H = normalize(V + lightDirection);

        float NdotL = max(dot(N, lightDirection), 0.0);
        float NdotV = max(dot(N, V), 0.0);
        float VdotH = max(dot(V, H), 0.0);

        vec3 F = fresnelSchlick(VdotH, F0);
        float D = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, lightDirection, roughness);

        vec3 denominator = 4.0 * NdotV * NdotL + vec3(0.001);
        vec3 specular = F * (D * G) / denominator;

        vec3 diffuse = NdotL * diffuseColor;
        vec3 lightContribution = (diffuse + specular) * light.color * light.intensity;
        totalLight += lightContribution;
    }

    // vec3 ambient = ambientColor;
    // vec3 color = totalLight + ambient;

    outColor = vec4(totalLight, 1.0);
}
