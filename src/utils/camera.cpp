#include "camera.hpp"

#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

void Camera::move(glm::vec2 movement) {
    glm::vec3 forward {std::cos(m_sphere_coord.z), 0., std::sin(m_sphere_coord.z)};
    glm::vec3 right = glm::normalize(glm::cross(forward, {0, 1, 0}));
    m_at += linear_speed * (forward * movement.y + right * movement.x) * scale_acc_factor * m_sphere_coord.x;
    m_eye_offset = sphereToCartesian(m_sphere_coord);
}

void Camera::rotate(glm::vec2 rotation) {
    m_sphere_coord.z += rotation.x * angular_speed.x;
    m_sphere_coord.y += rotation.y * angular_speed.y;
    m_sphere_coord.y = glm::min(pi - 0.01f, glm::max(0.01f, m_sphere_coord.y));
    m_eye_offset = sphereToCartesian(m_sphere_coord);
}

void Camera::zoom(float z) {
    m_sphere_coord.x += zoom_speed * z * scale_acc_factor * m_sphere_coord.x;
    m_eye_offset = sphereToCartesian(m_sphere_coord);
}

glm::mat4 Camera::viewMatrix() const {
    return glm::lookAt(m_eye_offset + m_at, m_at, m_up);
}

glm::mat4 Camera::projMatrix() const {
    return glm::perspectiveFov(fov, static_cast<float>(screen_size.x), static_cast<float>(screen_size.y), z_near, z_far);
}

glm::vec3 Camera::sphereToCartesian(glm::vec3 sphere) {
    return {
            std::cos(sphere.z) * std::sin(sphere.y) * sphere.x,
            std::cos(sphere.y) * sphere.x,
            std::sin(sphere.z) * std::sin(sphere.y) * sphere.x
    };
}