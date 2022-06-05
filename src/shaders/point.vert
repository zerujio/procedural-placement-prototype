#version 460

in vec3 a_position;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;
uniform float u_point_size = 2.0f;

void main() {
    gl_Position = u_proj * u_view * u_model * vec4(a_position, 1.0f);
    gl_PointSize = u_point_size;
}