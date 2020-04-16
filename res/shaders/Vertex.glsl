#version 450 core

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;

uniform vec4 ka;
uniform vec4 kd;
uniform vec4 ks;
uniform float shi;

uniform vec4 leye;
uniform mat4 mv;
uniform mat4 nm;
uniform mat4 mvp;

out vec4 color;
out vec4 color_s;

void main() {
    vec3 veye = vec3(mv * inPosition);
    vec4 light;
    if (leye.w == 0){
        light = normalize(leye);
    } else {
        light = normalize(leye - vec4(veye, 0.0f));
    }
    vec3 normal = normalize(vec3(nm * vec4(inNormal, 0.0f)));
    float ndotl = dot(normal, vec3(light));
    color = ka + max(0, ndotl) * kd;
    if (ndotl > 0){
        vec3 refl = normalize(reflect(vec3(-light), normal));
        color_s = ks * pow(max(0, dot(refl, normalize(-veye))), shi);
    } else {
        color_s = vec4(0, 0, 0, 1);
    }
    gl_Position = mvp * inPosition;
}