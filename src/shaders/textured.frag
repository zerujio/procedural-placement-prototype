#version 460

in vec3 f_normal;
in vec3 f_position;
in vec2 f_texCoord;

out vec4 final_color;

layout (binding = 0) uniform sampler2D u_texture;
uniform vec2 u_texScale = {1, 1};

layout (location = 3) uniform vec3 u_lightColor = {1.0f, 1.0f, 1.0f};
layout (location = 4) uniform vec3 u_lightDirection = {0.0f, 1.0f, 0.0f};
layout (location = 5) uniform float u_ambientStrength = 0.1f;
layout (location = 6) uniform float u_specularStrength = 0.5f;
layout (location = 7) uniform vec3 u_viewPosition = {0.0f, 0.0f, 0.0f};


void main() {
    vec3 normal = normalize(f_normal);
    float diffuse_strength = max(dot(normal, u_lightDirection), 0.0);

    vec3 view_direction = normalize(u_viewPosition - f_position);
    vec3 reflect_direction = reflect(-u_lightDirection, normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), 32);

    vec3 total_light_color = (u_ambientStrength + diffuse_strength + u_specularStrength * spec) * u_lightColor;
    vec3 object_color = texture(u_texture, f_texCoord * u_texScale).xyz;

    final_color = vec4(total_light_color * object_color, 1.0);
}