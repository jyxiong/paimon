#pragma once

#include "paimon/opengl/base/object.h"

namespace paimon {
// Vertex Array Object
// Vertex Array Attribute
// Vertex Array Binding
class VertexArray : public NamedObject {
public:
  VertexArray();
  ~VertexArray() override;

  VertexArray(const VertexArray &other) = delete;
  VertexArray &operator=(const VertexArray &other) = delete;

  bool is_valid() const override;

  void bind() const;
  static void unbind();
}

} // namespace paimon