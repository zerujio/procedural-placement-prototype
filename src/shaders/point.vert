#version 460

layout (location = 0) in vec3 a_position;

layout (location = 0) uniform mat4 u_model;
layout (location = 1) uniform mat4 u_view;
layout (location = 2) uniform mat4 u_projection;
layout (location = 3) uniform vec3 u_offset = {0., .001, 0.};

void main() {
    gl_Position = u_projection * u_view * (vec4(u_offset, 0.0f) + u_model * vec4(a_position, 1.0f));
}