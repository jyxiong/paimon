#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "paimon/opengl/buffer.h"
#include "paimon/opengl/state/vertex_input.h"
#include "paimon/opengl/type.h"

namespace paimon {
namespace sg {
/// Mesh primitive (single draw call unit)
struct Primitive {
  PrimitiveTopology mode = PrimitiveTopology::Triangles;

  size_t vertexCount = 0;
  std::shared_ptr<Buffer> positions;
  std::shared_ptr<Buffer> normals;
  std::shared_ptr<Buffer> texcoords;
  std::shared_ptr<Buffer> colors;

  size_t indexCount = 0;
  DataType indexType = DataType::UInt;
  std::shared_ptr<Buffer> indices;

  bool hasIndices() const { return indices != nullptr; }

  static std::vector<VertexInputState::Binding> bindings();

  static std::vector<VertexInputState::Attribute> attributes();

  static std::unique_ptr<Primitive> createCube();

  static std::unique_ptr<Primitive> createQuad();
};

struct Mesh {
  std::vector<Primitive> primitives;
};

} // namespace sg
} // namespace paimon
