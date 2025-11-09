#version 460

out vec4 final;

in vec2 texCoord;
uniform int divs;
uniform vec3 col1;
uniform vec3 col2;

void main() {
    vec3 result = vec3(0.0);

    float square = 2.0 / float(divs);
    vec2 edge = vec2(0.5);
    vec2 a = step(edge, fract(texCoord / square));

    int checkered = int(a.x + a.y) % 2;

    result = mix(col1, col2, float(checkered));

    final = vec4(1.0);
}