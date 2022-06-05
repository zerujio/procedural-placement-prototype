#version 460

out vec4 finalColor;

layout (location = 4) uniform vec4 u_color = {1.0f, 1.0f, 1.0f, 1.0f};

void main() {
    finalColor = u_color;
}
