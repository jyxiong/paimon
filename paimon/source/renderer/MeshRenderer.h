#pragma once

#include <memory>
#include "glm/glm.hpp"

#include "component/Component.h"
#include "component/Entity.h"

namespace Paimon
{

class Material;
class MeshFilter;

class MeshRenderer : public Component
{
public:
    explicit MeshRenderer(Entity &entity);
    ~MeshRenderer() override = default;

    void SetMaterial(const std::shared_ptr<Material> &material) { m_material = material; }
    std::shared_ptr<Material> GetMaterial() { return m_material; }

    void Render();

private:
    std::shared_ptr<Material> m_material;

    unsigned int m_vbo{};
    unsigned int m_ebo{};
    unsigned int m_vao{};

}; // class MeshRenderer

} // namespace Paimon
