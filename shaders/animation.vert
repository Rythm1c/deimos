#version 460

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tc;
layout(location = 3) in vec4 weights;
layout(location = 4) in ivec4 boneIds;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpace;

out vec3 normal;
out vec3 fragPos;
out vec2 texCoords;

const int MAX_BONES = 300;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 boneMats[MAX_BONES];

void main() {

    mat4 skin = boneMats[boneIds[0]] * weights[0];
    skin += boneMats[boneIds[1]] * weights[1];
    skin += boneMats[boneIds[2]] * weights[2];
    skin += boneMats[boneIds[3]] * weights[3];

    mat4 final_mat = transform * skin;
    gl_Position = projection * view * final_mat * vec4(pos, 1.0);

    normal = mat3(transpose(inverse(final_mat))) * norm;
    texCoords = tc;

    fragPos = vec3(final_mat * vec4(pos, 1.0));
   // vs_out.lightSpace = lightSpace * final_mat * vec4(pos, 1.0);

}
