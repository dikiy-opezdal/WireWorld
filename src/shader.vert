#version 330 core

layout(location=0) in vec2 a_pos;
layout(location=1) in vec2 a_offset;
layout(location=2) in vec3 a_color;

uniform float u_scale;
uniform vec2 u_camera_pos;
uniform vec2 u_resolution;

out vec3 v_color;

void main() {
    gl_Position = vec4((a_pos + (a_offset + u_camera_pos)*2) * u_scale / u_resolution, 0.0, 1.0);

    v_color = a_color;
}