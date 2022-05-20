#version 460

out vec4 finalColor;

layout(location=4) uniform vec4 u_color;

void main() {
    finalColor = u_color;
}
