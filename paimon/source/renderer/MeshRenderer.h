#pragma once

#include <memory>
#include "glm/glm.hpp"

#include "component/Component.h"
#include "component/GameObject.h"

namespace Paimon
{

class Material;
class MeshFilter;

class MeshRenderer : public Component
{
public:
    explicit MeshRenderer(GameObject &gameObject);
    ~MeshRenderer() override = default;

    void SetMaterial(const std::shared_ptr<Material> &material) { m_material = material; }
    std::shared_ptr<Material> GetMaterial() { return m_material; }

    void SetView(const glm::mat4 &view) { m_view = view; };
    void SetProjection(const glm::mat4 &projection) { m_projection = projection; };

    void Render();

private:
    std::shared_ptr<Material> m_material;

    glm::mat4 m_view{};
    glm::mat4 m_projection{};

    unsigned int m_vbo{};
    unsigned int m_ebo{};
    unsigned int m_vao{};

}; // class MeshRenderer

} // namespace Paimon
