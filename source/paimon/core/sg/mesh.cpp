#include "paimon/core/sg/mesh.h"

using namespace paimon;
using namespace sg;

std::vector<VertexInputState::Binding> Primitive::bindings() {
  return {
      {.binding = 0, .stride = sizeof(glm::vec3)}, // Position
      {.binding = 1, .stride = sizeof(glm::vec3)}, // Normal
      {.binding = 2, .stride = sizeof(glm::vec2)}, // TexCoord
      {.binding = 3, .stride = sizeof(glm::vec3)}, // Color
  };
}

std::vector<VertexInputState::Attribute> Primitive::attributes() {
  return {
      {
          .location = 0,
          .binding = 0,
          .format = GL_FLOAT,
          .size = 3, // vec3
          .offset = 0,
      },
      {
          .location = 1,
          .binding = 1,
          .format = GL_FLOAT,
          .size = 3, // vec3
          .offset = 0,
      },
      {
          .location = 2,
          .binding = 2,
          .format = GL_FLOAT,
          .size = 2, // vec2
          .offset = 0,
      },
      {
          .location = 3,
          .binding = 3,
          .format = GL_FLOAT,
          .size = 3, // vec3
          .offset = 0,
      },
  };
}