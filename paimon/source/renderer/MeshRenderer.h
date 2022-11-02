#pragma once

#include <memory>
#include "glm/glm.hpp"

namespace Paimon
{

class Material;
class MeshFilter;

class MeshRenderer
{
public:
    MeshRenderer() = default;
    ~MeshRenderer() = default;

    void SetMaterial(const std::shared_ptr<Material> &material) { m_material = material; }
    void SetMeshFilter(const std::shared_ptr<MeshFilter> &meshFilter) { m_meshFilter = meshFilter; }
    void SetMVP(const glm::mat4 &mvp) { m_mvp = mvp; }

    std::shared_ptr<Material> GetMaterial() { return m_material; }
    std::shared_ptr<MeshFilter> GetMeshFilter() { return m_meshFilter; }

    void Render();

private:
    std::shared_ptr<Material> m_material;
    std::shared_ptr<MeshFilter> m_meshFilter;
    glm::mat4 m_mvp;

    unsigned int m_vbo;
    unsigned int m_ebo;
    unsigned int m_vao;

}; // class MeshRenderer

} // namespace Paimon
