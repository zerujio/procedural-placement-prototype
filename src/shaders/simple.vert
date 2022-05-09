#version 460 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;

layout (location = 0) uniform mat4 u_model;
layout (location = 1) uniform mat4 u_view;
layout (location = 2) uniform mat4 u_projection;

out vec3 f_normal;
out vec3 f_position;
out vec3 f_color;

void main() {
    gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);

    f_normal = mat3(transpose(inverse(u_model))) * a_normal;
    f_position = vec3(u_model * vec4(a_position, 1.0));
    f_color = abs(a_normal);
}