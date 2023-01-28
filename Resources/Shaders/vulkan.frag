// FRAGMENT SHADER
#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;

layout(set = 1, binding = 0) uniform LightProperties {
vec4 lightColor;
vec3 lightPos;
} light;

layout(set = 2, binding = 0) uniform sampler2D diffuseMap;
layout(set = 2, binding = 1) uniform sampler2D normalMap;
layout(set = 2, binding = 2) uniform sampler2D metallicMap;
layout(set = 2, binding = 3) uniform sampler2D roughnessMap;
layout(set = 2, binding = 4) uniform sampler2D ambientMap;

layout(location = 0) out vec4 outColor;

float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
float a = roughness*roughness;
float a2 = a*a;
float NdotH = max(dot(N, H), 0.0);
float NdotH2 = NdotH*NdotH;

float num = a2;
float denom = (NdotH2 * (a2 - 1.0) + 1.0);
denom = PI * denom * denom;

return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
float r = (roughness + 1.0);
float k = (r*r) / 8.0;

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

vec3 IntegrateBRDF(float NdotV, float roughness) {
vec3 V;
V.x = sqrt(1.0 - NdotV*NdotV);
V.y = 0.0;
V.z = NdotV;

float A = 0.0;
float B = 0.0;

vec3 N = vec3(0.0, 0.0, 1.0);

const uint SAMPLE_COUNT = 1024u;
for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
float phi = 2.0 * PI * float(i) / float(SAMPLE_COUNT);
vec2 Xi = vec2(cos(phi), sin(phi));

float cosTheta = sqrt((1.0 - Xi.x) / (1.0 + (roughness*roughness - 1.0) * Xi.x));
float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

vec3 H = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
vec3 L = normalize(2.0 * dot(V, H) * H - V);

float NdotL = max(L.z, 0.0);
float NdotH = max(H.z, 0.0);
float VdotH = max(dot(V, H), 0.0);

if (NdotL > 0.0) {
float G = GeometrySmith(N, V, L, roughness);
float G_Vis = (G * VdotH) / (NdotH * NdotV);
float Fc = pow(1.0 - VdotH, 5.0);

A += (1.0 - Fc) * G_Vis;
B += Fc * G_Vis;
}
}

return vec3(A, B, 0.0) / float(SAMPLE_COUNT);
}

void main() {
vec3 albedo = texture(diffuseMap, fragTexCoord).rgb;
vec3 normal = texture(normalMap, fragTexCoord).rgb;
float metallic = texture(metallicMap, fragTexCoord).r;
float roughness = texture(roughnessMap, fragTexCoord).r;
float ao = texture(ambientMap, fragTexCoord).r;

vec3 N = normalize(fragNormal);
vec3 V = normalize(light.lightPos - gl_FragCoord.xyz);
vec3 L = normalize(light.lightPos - gl_FragCoord.xyz);

vec3 H = normalize(V + L);
float NdotL = max(dot(N, L), 0.0);

vec3 F0 = vec3(0.04);
F0 = mix(F0, albedo, metallic);

vec3 Lo = vec3(0.0);
vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
float G = GeometrySmith(N, V, L, roughness);
float D = DistributionGGX(N, H, roughness);

vec3 kS = F;
vec3 kD = 1.0 - kS;
kD *= 1.0 - metallic;

float NdotV = max(dot(N, V), 0.0);
vec3 specular = F * G * D / (4.0 * NdotL * NdotV + 0.001);

vec3 ambient = (albedo * ao) * 0.1;

vec3 radiance = NdotL * (kD * albedo / PI + specular) * light.lightColor.rgb;

outColor = vec4(radiance + ambient, 1.0);
}
