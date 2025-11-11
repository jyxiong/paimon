#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace paimon {
namespace sg {

struct Material;

/// Mesh primitive (single draw call unit)
struct Primitive {
  /// Vertex attribute data
  struct Attribute {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec4> tangents; // xyz = tangent, w = handedness
    std::vector<glm::vec2> texcoords_0;
    std::vector<glm::vec2> texcoords_1;
    std::vector<glm::vec4> colors_0;
    std::vector<glm::vec4> joints_0;  // For skinning (not used yet)
    std::vector<glm::vec4> weights_0; // For skinning (not used yet)

    bool HasPositions() const { return !positions.empty(); }
    bool HasNormals() const { return !normals.empty(); }
    bool HasTangents() const { return !tangents.empty(); }
    bool HasTexCoords0() const { return !texcoords_0.empty(); }
    bool HasTexCoords1() const { return !texcoords_1.empty(); }
    bool HasColors() const { return !colors_0.empty(); }

    size_t GetVertexCount() const { return positions.size(); }
  };

  Attribute attributes;
  std::vector<uint32_t> indices;

  std::shared_ptr<Material> material = nullptr;

  // Primitive topology
  enum class Mode {
    Points = 0,
    Lines = 1,
    LineLoop = 2,
    LineStrip = 3,
    Triangles = 4,
    TriangleStrip = 5,
    TriangleFan = 6
  };
  Mode mode = Mode::Triangles;

  bool HasIndices() const { return !indices.empty(); }
};

/// Mesh containing one or more primitives
struct Mesh {
  std::string name;
  std::vector<Primitive> primitives;

  Mesh() = default;
  Mesh(const std::string &name) : name(name) {}
};

} // namespace sg
} // namespace paimon
