#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "paimon/core/sg/material.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/type.h"

namespace paimon {
namespace sg {
/// Mesh primitive (single draw call unit)
struct Primitive {
  PrimitiveTopology mode = PrimitiveTopology::Triangles;

  std::unique_ptr<Buffer> positions;
  std::unique_ptr<Buffer> normals;
  std::unique_ptr<Buffer> texcoords;
  std::unique_ptr<Buffer> colors;
  std::unique_ptr<Buffer> indices;

  std::shared_ptr<Material> material = nullptr;

  bool hasIndices() const { return indices != nullptr; }
};

struct Mesh {
  std::vector<Primitive> primitives;
};

} // namespace sg
} // namespace paimon
