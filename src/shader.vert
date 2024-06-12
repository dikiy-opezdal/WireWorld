#version 330 core

layout(location=0) in vec2 a_pos;

uniform vec2 u_offset;
uniform float u_scale;
uniform vec2 u_resolution;

void main() {
    gl_Position = vec4((a_pos * u_scale + u_offset) / u_resolution, 0.0, 1.0);
}