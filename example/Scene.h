#pragma once

#include "glm/glm.hpp"

#include "component/Component.h"

namespace Paimon
{
class Material;

class Scene : public Component
{
public:
    explicit Scene(Entity &entity);
    ~Scene() override = default;

    void Awake() override;
    void Update() override;

private:
    void CreateFishSoupPot();

    void CreateQuad();

private:
    std::shared_ptr<Entity> m_fishSoup;
    std::shared_ptr<Entity> m_editorCamera;

    std::shared_ptr<Material> m_fontMaterial;

    glm::vec2 m_mousePosition{};

}; // class Renderer

} // namespace Paimon
