#version 460

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tc;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 fragPos;
out vec2 texCoords;

void main() {

    fragPos = vec3(transform * vec4(pos, 1.0));
    normal = mat3(transpose(inverse(transform))) * norm;
    texCoords = tc;

    gl_Position = projection * view * transform * vec4(pos, 1.0);

}