#pragma once

#include "glm/glm.hpp"

#include "component/Component.h"

namespace Paimon
{

class Renderer : public Component
{
public:
    explicit Renderer(Entity &entity);
    ~Renderer() override = default;

    void Awake() override;
    void Update() override;

private:
    std::shared_ptr<Entity> m_fishSoup;
    std::shared_ptr<Entity> m_sceneCamera;
    std::shared_ptr<Entity> m_editorCamera;

    glm::vec2 m_mousePosition{};

}; // class Renderer

} // namespace Paimon
