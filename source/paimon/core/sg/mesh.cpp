#include "paimon/core/sg/mesh.h"
#include <memory>

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

std::unique_ptr<Primitive> Primitive::createCube() {
    auto primitive = std::make_unique<Primitive>();
    
    float halfSize = 1.f;
    
    // Define cube vertices
    glm::vec3 positions[] = {
        // Front face
        {-halfSize, -halfSize,  halfSize},
        { halfSize, -halfSize,  halfSize},
        { halfSize,  halfSize,  halfSize},
        {-halfSize,  halfSize,  halfSize},
        // Back face
        {-halfSize, -halfSize, -halfSize},
        {-halfSize,  halfSize, -halfSize},
        { halfSize,  halfSize, -halfSize},
        { halfSize, -halfSize, -halfSize},
        // Left face
        {-halfSize, -halfSize, -halfSize},
        {-halfSize, -halfSize,  halfSize},
        {-halfSize,  halfSize,  halfSize},
        {-halfSize,  halfSize, -halfSize},
        // Right face
        { halfSize, -halfSize, -halfSize},
        { halfSize,  halfSize, -halfSize},
        { halfSize,  halfSize,  halfSize},
        { halfSize, -halfSize,  halfSize},
        // Top face
        {-halfSize,  halfSize, -halfSize},
        {-halfSize,  halfSize,  halfSize},
        { halfSize,  halfSize,  halfSize},
        { halfSize,  halfSize, -halfSize},
        // Bottom face
        {-halfSize, -halfSize, -halfSize},
        { halfSize, -halfSize, -halfSize},
        { halfSize, -halfSize,  halfSize},
        {-halfSize, -halfSize,  halfSize},
    };
    
    // Define cube indices
    uint32_t indices[] = {
        0, 1, 2, 2, 3, 0,       // Front face
        4, 5, 6, 6, 7, 4,       // Back face
        8, 9,10,10,11, 8,       // Left face
       12,13,14,14,15,12,       // Right face
       16,17,18,18,19,16,       // Top face
       20,21,22,22,23,20        // Bottom face
    };

    primitive->positions = std::make_unique<Buffer>();
    primitive->positions->set_storage(sizeof(positions), positions, 0);

    primitive->indices = std::make_unique<Buffer>();
    primitive->indices->set_storage(sizeof(indices), indices, 0);
    
    return primitive;
}

std::unique_ptr<Primitive> Primitive::createQuad() {
    auto primitive = std::make_unique<Primitive>();
    
    float halfSize = 1.f;
    
    // Define quad vertices
    glm::vec3 positions[] = {
        {-halfSize, -halfSize, 0.0f},
        { halfSize, -halfSize, 0.0f},
        { halfSize,  halfSize, 0.0f},
        {-halfSize,  halfSize, 0.0f},
    };

    glm::vec2 texCoords[] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
    };
    
    // Define quad indices
    uint32_t indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    primitive->positions = std::make_unique<Buffer>();
    primitive->positions->set_storage(sizeof(positions), positions, 0);

    primitive->texcoords = std::make_unique<Buffer>();
    primitive->texcoords->set_storage(sizeof(texCoords), texCoords, 0);

    primitive->indices = std::make_unique<Buffer>();
    primitive->indices->set_storage(sizeof(indices), indices, 0);
    
    return primitive;
}