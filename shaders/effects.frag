#version 450

uniform sampler2D frame;

in vec2 outTexCoord;

out vec4 color;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// for barrel distortion
uniform float barrelPower;
uniform bool distortion;

vec2 distort(vec2 p) {

    float theta = atan(p.y, p.x);
    float radius = length(p);
    radius = pow(radius, barrelPower);

    p.x = radius * cos(theta);
    p.y = radius * sin(theta);

    return 0.5 * (p + 1.0);
}

uniform bool grain;

vec3 graining() {

    vec3 result = vec3(1.0);

    vec2 st = outTexCoord / vec2(1.0);

    float noise = random(st);

    result = result * noise;

    return result;
}

void main() {
    vec3 result = vec3(0.0);
    if(distortion) {
        vec2 xy = 2.0 * outTexCoord - 1.0;
        vec2 uv;
        float d = length(xy);

        if(d < 1.0) {
            uv = distort(xy);
        } else {
            uv = outTexCoord;
        }
        result = texture(frame, uv).rgb;
    } else {
        result = texture(frame, outTexCoord).rgb;
    }
    if(grain) {
        result *= graining();
    }
    color = vec4(result, 1.0);
    //color=vec4(0.55, 0.33, 0.22, 1.0);
}