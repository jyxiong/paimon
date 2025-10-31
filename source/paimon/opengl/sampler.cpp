#include "paimon/opengl/sampler.h"
#include "glad/gl.h"

using namespace paimon;

Sampler::Sampler() : NamedObject(GL_SAMPLER) {
  glCreateSamplers(1, &m_name);
}

Sampler::~Sampler() {
  if (m_name != 0) {
    glDeleteSamplers(1, &m_name);
  }
}

bool Sampler::is_valid() const { return glIsSampler(m_name) == GL_TRUE; }

void Sampler::bind(GLuint unit) { glBindSampler(unit, m_name); }

template <> void Sampler::get<GLint>(GLenum property, GLint *value) {
  glGetSamplerParameteriv(m_name, property, value);
}

template <> void Sampler::get<GLfloat>(GLenum property, GLfloat *value) {
  glGetSamplerParameterfv(m_name, property, value);
}

template <> GLint Sampler::get<GLint>(GLenum property) {
  GLint value;
  get(property, &value);
  return value;
}

template <> GLfloat Sampler::get<GLfloat>(GLenum property) {
  GLfloat value;
  get(property, &value);
  return value;
}

template <> void Sampler::set<GLint>(GLenum property, GLint value) {
  glSamplerParameteri(m_name, property, value);
}

template <> void Sampler::set<GLenum>(GLenum property, GLenum value) {
  glSamplerParameteri(m_name, property, static_cast<GLint>(value));
}

template <> void Sampler::set<GLfloat>(GLenum property, GLfloat value) {
  glSamplerParameterf(m_name, property, value);
}

template <> void Sampler::set<GLint *>(GLenum property, GLint *value) {
  glSamplerParameteriv(m_name, property, value);
}

template <>
void Sampler::set<GLfloat *>(GLenum property, GLfloat *value) {
  glSamplerParameterfv(m_name, property, value);
}

template <>
void Sampler::set<const GLint *>(GLenum property, const GLint *value) {
  glSamplerParameteriv(m_name, property, value);
}

template <>
void Sampler::set<const GLfloat *>(GLenum property,
                                          const GLfloat *value) {
  glSamplerParameterfv(m_name, property, value);
}
