#pragma once

#include <memory>

#include "paimon/core/sg/material.h"
#include "paimon/opengl/buffer.h"

namespace paimon {
struct MeshData {
  Buffer position_buffer;
  Buffer normal_buffer;
  Buffer texcoord_buffer;
  Buffer index_buffer;
  size_t index_count;
  std::shared_ptr<sg::Material> material;
};
} // namespace paimon