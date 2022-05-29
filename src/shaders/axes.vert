#version 460

in vec3 a_position;
in vec3 a_color;

out vec3 f_color;

layout(location = 0) uniform mat4 u_view;
layout(location = 1) uniform mat4 u_proj;

void main() {
    gl_Position = u_proj * u_view * vec4(a_position, 1.0f);
    f_color = a_color;
}
