#include "paimon/opengl/query.h"

#include "glad/gl.h"
#include "paimon/opengl/base/object.h"

using namespace paimon;

Query::Query(GLenum type) : NamedObject(GL_QUERY), m_type(type) {
}

Query::~Query() {
}

void Query::create() {
  if (m_name == 0) {
    glCreateQueries(m_type, 1, &m_name);
  }
}

void Query::destroy() {
  if (m_name != 0) {
    glDeleteQueries(1, &m_name);
    m_name = 0;
  }
}

bool Query::is_valid() const { return glIsQuery(m_name) == GL_TRUE; }

GLenum Query::get_type() const { return m_type; }

void Query::begin() { glBeginQuery(m_type, m_name); }

void Query::end() { glEndQuery(m_type); }

template <> void Query::get<GLint>(GLenum property, GLint *rslt) {
  glGetQueryObjectiv(m_name, property, rslt);
}

template <> void Query::get<GLuint>(GLenum property, GLuint *rslt) {
  glGetQueryObjectuiv(m_name, property, rslt);
}

template <> void Query::get<GLint64>(GLenum property, GLint64 *rslt) {
  glGetQueryObjecti64v(m_name, property, rslt);
}

template <> void Query::get<GLuint64>(GLenum property, GLuint64 *rslt) {
  glGetQueryObjectui64v(m_name, property, rslt);
}

template <> GLint Query::get<GLint>(GLenum property) {
  GLint rslt;
  glGetQueryObjectiv(m_name, property, &rslt);
  return rslt;
}

template <> GLuint Query::get<GLuint>(GLenum property) {
  GLuint rslt;
  glGetQueryObjectuiv(m_name, property, &rslt);
  return rslt;
}

template <> GLint64 Query::get<GLint64>(GLenum property) {
  GLint64 rslt;
  glGetQueryObjecti64v(m_name, property, &rslt);
  return rslt;
}

template <> GLuint64 Query::get<GLuint64>(GLenum property) {
  GLuint64 rslt;
  glGetQueryObjectui64v(m_name, property, &rslt);
  return rslt;
}