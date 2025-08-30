#pragma once

#include <memory>

#include "paimon/opengl/buffer.h"
#include "paimon/opengl/program.h"
#include "paimon/opengl/sampler.h"
#include "paimon/opengl/texture.h"
#include "paimon/opengl/vertex_array.h"

using namespace paimon;

class ScreenQuad {
public:
  ScreenQuad();

  void draw(const Texture &texture);

protected:
  std::unique_ptr<Buffer> m_vertex_buffer;
  std::unique_ptr<Buffer> m_index_buffer;

  std::unique_ptr<VertexArray> m_vao;

  std::unique_ptr<Program> m_program;

  std::unique_ptr<Sampler> m_sampler;

protected:
  static const float s_vertices[20];
  static const unsigned int s_indices[6];
  static const char *s_vertexShaderSource;
  static const char *s_fragmentShaderSource;
};