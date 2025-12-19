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

  std::shared_ptr<Buffer> positions;
  std::shared_ptr<Buffer> normals;
  std::shared_ptr<Buffer> texcoords;
  std::shared_ptr<Buffer> colors;
  std::shared_ptr<Buffer> indices;

  std::shared_ptr<Material> material = nullptr;

  bool hasIndices() const { return indices != nullptr; }
};

struct Mesh {
  std::vector<Primitive> primitives;
};

} // namespace sg
} // namespace paimon
