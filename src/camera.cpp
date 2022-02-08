#include "paimon/camera.h"
#include "glm/gtc/matrix_transform.hpp"

using namespace paimon;

Camera::Camera(const glm::vec3& position, const glm::vec3& focal, const glm::vec3& view_up)
    : m_position(position), m_focal(focal), m_view_up(view_up) {}


void Camera::set_perspective(float vertical_fov, float near_clip, float far_clip) {
    m_projection_type = ProjectionType::Perspective;
    m_perspective_fov = vertical_fov;
    m_perspective_near = near_clip;
    m_perspective_far = far_clip;
}

void Camera::set_orthographic(float size, float near_clip, float far_clip) {
    m_projection_type = ProjectionType::Orthographic;
    m_orthographic_size = size;
    m_orthographic_near = near_clip;
    m_orthographic_far = far_clip;
}

void Camera::set_viewport_size(uint32_t width, uint32_t height) {
    m_aspect_ratio = (float)width / (float)height;
}

const glm::mat4& Camera::get_projection_matrix() {
    if (m_projection_type == ProjectionType::Perspective) {
        m_projection_matrix = glm::perspective(m_perspective_fov, m_aspect_ratio, m_perspective_near, m_perspective_far);
    } else {
        float ortho_left = -m_orthographic_size * m_aspect_ratio * 0.5f;
        float ortho_right = m_orthographic_size * m_aspect_ratio * 0.5f;
        float ortho_bottom = -m_orthographic_size * 0.5f;
        float ortho_top = m_orthographic_size * 0.5f;

        m_projection_matrix = glm::ortho(ortho_left, ortho_right, ortho_bottom, ortho_top, m_orthographic_near, m_orthographic_far);
    }
    return m_projection_matrix;
}

const glm::mat4& Camera::get_view_matrix() {
    m_view_matrix = glm::lookAt(m_position, m_focal, m_view_up);
    return m_view_matrix;
}
