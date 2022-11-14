#pragma once

#include <filesystem>
#include <memory>
#include <vector>

#include "glm/glm.hpp"

#include "component/Component.h"
#include "component/Entity.h"

namespace Paimon
{
struct Vertex
{
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 uv;
}; // struct Vertex

struct MeshFileHead
{
    char extension[4];
    unsigned short numVertex;
    unsigned short numIndex;
}; // struct MeshFileHead

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<unsigned short> indices;
}; // struct Mesh

class MeshFilter : public Component
{
public:
    explicit MeshFilter(Entity &entity);
    ~MeshFilter() override = default;

    std::shared_ptr<Mesh> GetMesh() { return m_mesh; };

    void LoadMesh(const std::filesystem::path &path);

    void CreateMesh(std::vector<Vertex> &vertices, std::vector<unsigned short> &indices);

private:
    std::shared_ptr<Mesh> m_mesh;

}; // class MeshFilter
} // namespace Paimon
