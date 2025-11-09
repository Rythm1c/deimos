//physical besed rendering fragment shader
#version 460

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;

#define MAX_LIGHTS 20
uniform struct Light {
    vec3 color;
    vec3 position;
} lights[MAX_LIGHTS];
uniform int lightCount;

uniform vec3 camPos;

/*** material defination ***/
uniform vec4 baseColor; // or emissive factor
uniform float metallicFactor;
uniform float roughness;
uniform float ao;

uniform sampler2D albedoMap;
uniform sampler2D metallicMap;
uniform sampler2D normalMap;
uniform bool hasBaseTexture;
uniform bool hasMetallicMap;

out vec4 color;

float distributionGGX(vec3, vec3, float);
float GeometrySchlickGGX(float, float);
float geometrySmith(vec3, vec3, vec3, float);
vec3 frenselSchlick(float, vec3);

const float PI = 3.14159265359;

// blending with background based on distance from camera
// also can be used to create a lazy fog effect
float blend(float far);

//_________________________________________________________________________
void main() {
    vec3 albedo = pow(baseColor.xyz, vec3(2.2));
    if(hasBaseTexture) {
        albedo = pow(texture(albedoMap, texCoords).rgb, vec3(2.2));
    }

    float metallic = metallicFactor;
    if(hasMetallicMap) {
        metallic = texture(metallicMap, texCoords).r;
    }

    vec3 N = normalize(normal);
    vec3 V = normalize(camPos - fragPos);

    vec3 f0 = vec3(0.04);
    f0 = mix(f0, albedo, metallic);

    vec3 lo = vec3(0.0);
    for(int i = 0; i < lightCount; i++) {
        vec3 L = normalize(lights[i].position - fragPos);
        vec3 H = normalize(V + L);

        //the attenuation works alittle too well...

        float distance = length(lights[i].position - fragPos);
        float attenuation = 1.0 / (distance * 2.0);
        vec3 radiance = lights[i].color * attenuation;

        float NDF = distributionGGX(N, H, roughness);
        float G = geometrySmith(N, V, L, roughness);
        vec3 F = frenselSchlick(clamp(dot(H, V), 0.0, 1.0), f0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 KS = F;

        vec3 KD = vec3(1.0) - KS;

        KD *= 1.0 - metallic;

        float NdotL = max(dot(N, L), 0.0);

        lo += (KD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 result = ambient + lo;

    // HDR tonemapping
    result = result / (result + vec3(1.0));

    // gamma correction
    result = pow(result, vec3(1.0 / 2.2));

    color = vec4(result, 1.0);
}

//*** function deinations **//
//_________________________________________________________________________
float distributionGGX(vec3 N, vec3 H, float roughness) {
    float a = pow(roughness, 2.0);
    float a2 = pow(a, 2.0);
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = pow(NdotH, 2.0);

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * pow(denom, 2.0);

    return (nom / denom);
}
//_________________________________________________________________________
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = pow(r, 2.0) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return (nom / denom);
}
//_________________________________________________________________________
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);

    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return (ggx1 * ggx2);
}
//_________________________________________________________________________
vec3 frenselSchlick(float cosTheta, vec3 f0) {
    return f0 + (1.0 - f0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
//_________________________________________________________________________
float blend(float far) {
    float distance = clamp(length(fragPos - camPos), 0.0, far);
    return (pow(distance / far, 2.0));
}
