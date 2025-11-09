 //specialized shader to create checker board like texture
#version 460

out vec4 final;

in vec2 textCoord;

uniform vec3 mainCol;
uniform vec3 gridCol;

uniform float thickness;
uniform int lats;
uniform int longs;

void main() {
    vec3 result = vec3(0.0);

    vec2 edge = vec2(thickness);

    float latStep = 1.0 / float(lats);
    float longStep = 1.0 / float(longs);

    float u = fract(textCoord.x / longStep);
    float v = fract(textCoord.y / latStep);

    vec2 a = step(edge, vec2(u, v));
    vec2 b = step(edge, 1.0 - vec2(u, v));

    float inGrid = a.x * b.x * a.y * b.y;

    result = mix(gridCol, mainCol, inGrid);

    final = vec4(1.0);

}