#version 450

in vec2 texCoord;

out vec4 t_color;

uniform sampler2D text;
uniform vec3 textColor;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, texCoord).r);
    t_color = vec4(textColor, 1.0) * sampled;
}