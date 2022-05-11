#version 460

layout (location = 0) in vec3 a_position;

layout (location = 0) uniform mat4 u_model;
layout (location = 1) uniform mat4 u_view;
layout (location = 2) uniform mat4 u_projection;

void main() {
    //gl_Position = vec4(a_position, 1.0f).xzyw;
    gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0f);
}