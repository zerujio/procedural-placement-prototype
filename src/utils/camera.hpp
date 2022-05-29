#ifndef PROCEDURALPLACEMENT_CAMERA_HPP
#define PROCEDURALPLACEMENT_CAMERA_HPP

#include <glm/gtc/constants.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Camera {

    static constexpr auto pi = glm::pi<float>();

public:
    glm::vec2 angular_speed {pi / 2, pi / 4};
    float linear_speed {1.5f};
    float zoom_speed {10.0f};
    float scale_acc_factor {.75f};

    glm::ivec2 screen_size {1024, 768};
    float fov = pi / 4;
    float z_near = 0.01f;
    float z_far = 1000.0f;

    void move(glm::vec2 movement);

    void rotate(glm::vec2 rotation);

    void zoom(float z);

    [[nodiscard]]
    glm::mat4 viewMatrix() const;

    [[nodiscard]]
    glm::mat4 projMatrix() const;

    [[nodiscard]]
    glm::vec3 at() const {
        return m_at;
    }

    [[nodiscard]]
    glm::vec3 eye() const {
        return m_at + m_eye_offset;
    }

private:

    static glm::vec3 sphereToCartesian(glm::vec3 sphere);

    glm::vec3 m_sphere_coord {2.5, pi / 4, 0.};
    glm::vec3 m_at {0.0f};
    glm::vec3 m_up {0., 1., 0.};
    glm::vec3 m_eye_offset {sphereToCartesian(m_sphere_coord)};
};


#endif //PROCEDURALPLACEMENT_CAMERA_HPP
