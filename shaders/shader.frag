#version 460

out vec4 ouput;

uniform vec3 lightDirection;
uniform vec3 viewPos;
uniform vec3 baseColor;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;

// returns a fraction of the far value depending on the distance from the camera
float blend(float far);

uniform bool hasBaseTex;
uniform bool hasSpecMap;
uniform sampler2D baseTex;
uniform sampler2D specMap;
// beginning of main function
void main() {
    vec3 result = vec3(0.0);

    vec3 color = baseColor;
    if (hasBaseTex) {
        color = vec3(texture(baseTex, texCoords));
    }

    vec3 ambient = vec3(0.1) * color;
    result += ambient;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-lightDirection);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color;
    result += diffuse;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwaydir = normalize(lightDir + viewDir);
    // vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(norm, halfwaydir), 0.0), 128.0);
    vec3 specular = spec * color;
    result += specular;

    /* float attenuation = pow(blend(200.0), 2.0);
     result = mix(result, vec3(0.2), attenuation);
      */

    ouput = vec4(result, 1.0);
    //ouput = vec4(1.0, 0.58, 0.1, 0.0);
}

float blend(float far) {
    float dist = clamp(length(viewPos - fragPos), 0.0, far);
    return dist / far;
}
