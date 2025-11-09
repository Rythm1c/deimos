#version 450

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 texCoord;

uniform mat4 ortho;
out vec2 outTexCoord;

void main() {
    outTexCoord = texCoord;
    gl_Position = ortho * vec4(pos, 0.0, 1.0);
}
