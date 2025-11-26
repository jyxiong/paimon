#pragma once

#include <memory>

#include "paimon/opengl/program.h"
#include "paimon/opengl/sampler.h"
#include "paimon/opengl/texture.h"
#include "paimon/opengl/vertex_array.h"

using namespace paimon;

class ScreenQuad {
public:
  ScreenQuad();

  void setProgram(Program *program);
  void draw(const Texture &texture);

private:
  std::unique_ptr<VertexArray> m_vao;
  std::unique_ptr<Sampler> m_sampler;
  Program *m_program = nullptr;
};
