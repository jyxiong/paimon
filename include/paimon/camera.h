#pragma once

#include "glm/glm.hpp"

namespace paimon {
    class Camera {
    public:
        Camera() = default;
        Camera(const glm::vec3& position, const glm::vec3& focal, const glm::vec3& view_up);

        void set_perspective(float vertical_fov, float near_clip, float far_clip);
        void set_orthographic(float size, float near_clip, float far_clip);

        void set_viewport_size(uint32_t width, uint32_t height);

        const glm::mat4& get_projection_matrix();
        const glm::mat4& get_view_matrix();

    private:
        // view matrix
        glm::mat4 m_view_matrix = glm::mat4(1.0f);
        glm::vec3   m_position;
        glm::vec3   m_focal;
        glm::vec3   m_view_up;

        // projection matrix
        glm::mat4 m_projection_matrix = glm::mat4(1.0f);

        float m_aspect_ratio = 0.0f;

        enum class ProjectionType {
            Perspective,
            Orthographic
        };
        ProjectionType m_projection_type = ProjectionType::Orthographic;

        float m_perspective_fov = glm::radians(45.0f);
        float m_perspective_near = 0.01f;
        float m_perspective_far = 1000.0f;

        float m_orthographic_size = 10.0f;
        float m_orthographic_near = -1.0f;
        float m_orthographic_far = 1.0f;

    }; // class camera
} // namespace paimon