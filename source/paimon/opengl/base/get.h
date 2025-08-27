#pragma once

#include "glad/gl.h"

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGet.xhtml

namespace paimon {

template <class T> void get(GLenum parameter, T *value);

template <> inline void get<GLboolean>(GLenum parameter, GLboolean *value) {
  glGetBooleanv(parameter, value);
}

template <> inline void get<GLfloat>(GLenum parameter, GLfloat *value) {
  glGetFloatv(parameter, value);
}

template <> inline void get<GLdouble>(GLenum parameter, GLdouble *value) {
  glGetDoublev(parameter, value);
}

template <> inline void get<GLint>(GLenum parameter, GLint *value) {
  glGetIntegerv(parameter, value);
}

template <> inline void get<GLint64>(GLenum parameter, GLint64 *value) {
  glGetInteger64v(parameter, value);
}

template <class T> T get(GLenum parameter);

template <> inline GLboolean get<GLboolean>(GLenum parameter) {
  GLboolean value;
  get(parameter, &value);
  return value;
}

template <> inline GLfloat get<GLfloat>(GLenum parameter) {
  GLfloat value;
  get(parameter, &value);
  return value;
}

template <> inline GLdouble get<GLdouble>(GLenum parameter) {
  GLdouble value;
  get(parameter, &value);
  return value;
}

template <> inline GLint get<GLint>(GLenum parameter) {
  GLint value;
  get(parameter, &value);
  return value;
}

template <> inline GLint64 get<GLint64>(GLenum parameter) {
  GLint64 value;
  get(parameter, &value);
  return value;
}

template <class T> void get(GLenum parameter, GLuint index, T *value);

template <>
inline void get<GLboolean>(GLenum parameter, GLuint index, GLboolean *value) {
  glGetBooleani_v(parameter, index, value);
}

template <>
inline void get<GLfloat>(GLenum parameter, GLuint index, GLfloat *value) {
  glGetFloati_v(parameter, index, value);
}

template <>
inline void get<GLdouble>(GLenum parameter, GLuint index, GLdouble *value) {
  glGetDoublei_v(parameter, index, value);
}

template <>
inline void get<GLint>(GLenum parameter, GLuint index, GLint *value) {
  glGetIntegeri_v(parameter, index, value);
}

template <>
inline void get<GLint64>(GLenum parameter, GLuint index, GLint64 *value) {
  glGetInteger64i_v(parameter, index, value);
}

template <class T> T get(GLenum parameter, GLuint index);

template <> inline GLboolean get<GLboolean>(GLenum parameter, GLuint index) {
  GLboolean value;
  get(parameter, index, &value);
  return value;
}

template <> inline GLfloat get<GLfloat>(GLenum parameter, GLuint index) {
  GLfloat value;
  get(parameter, index, &value);
  return value;
}

template <> inline GLdouble get<GLdouble>(GLenum parameter, GLuint index) {
  GLdouble value;
  get(parameter, index, &value);
  return value;
}

template <> inline GLint get<GLint>(GLenum parameter, GLuint index) {
  GLint value;
  get(parameter, index, &value);
  return value;
}

template <> inline GLint64 get<GLint64>(GLenum parameter, GLuint index) {
  GLint64 value;
  get(parameter, index, &value);
  return value;
}

} // namespace paimon