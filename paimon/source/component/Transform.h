#pragma once

#include "glm/glm.hpp"

#include "Component.h"
#include "GameObject.h"

namespace Paimon
{

class Transform : public Component
{
public:
    explicit Transform(GameObject &gameObject);
    ~Transform() override = default;

    [[nodiscard]] const glm::vec3 &GetPosition() const { return m_position; }
    [[nodiscard]] const glm::vec3 &GetRotation() const { return m_rotation; }
    [[nodiscard]] const glm::vec3 &GetScale() const { return m_scale; }

    void SetPosition(const glm::vec3 &position) { m_position = position; }
    void SetRotation(const glm::vec3 &rotation) { m_rotation = rotation; }
    void SetScale(const glm::vec3 &scale) { m_scale = scale; }

private:
    glm::vec3 m_position{0.f};
    glm::vec3 m_rotation{0.f};
    glm::vec3 m_scale{1.f};

}; // class Transform

} // namespace Paimon
