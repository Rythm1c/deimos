#version 460

vec2 positions[6] = vec2[](
//top left
vec2(-1.0, 1.0),
//top right
vec2(1.0, 1.0),
//bottom left
vec2(-1.0, -1.0), 
//top right
vec2(1.0, 1.0), 
//bottom left
vec2(-1.0, -1.0), 
//bottom right 
vec2(1.0, -1.0));

vec2 texCoords[6] = vec2[](vec2(0.0, 1.0), vec2(1.0, 1.0), vec2(0.0, 0.0), vec2(1.0, 1.0), vec2(0.0, 0.0), vec2(1.0, 0.0));

out vec2 textCoord;

void main() {

    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    texCoord = texCoords[gl_VertexIndex];
}