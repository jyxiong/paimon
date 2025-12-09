#pragma once

#include <concepts>

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGet.xhtml

namespace paimon {

template <typename T>
auto get(GLenum pname) {
  if constexpr (std::same_as<T, GLboolean>) {
    GLboolean value;
    glGetBooleanv(pname, &value);
    return value;
  } else if constexpr (std::same_as<T, GLfloat>) {
    GLfloat value;
    glGetFloatv(pname, &value);
    return value;
  } else if constexpr (std::same_as<T, GLdouble>) {
    GLdouble value;
    glGetDoublev(pname, &value);
    return value;
  } else if constexpr (std::same_as<T, GLint>) {
    GLint value;
    glGetIntegerv(pname, &value);
    return value;
  } else if constexpr (std::same_as<T, GLint64>) {
    GLint64 value;
    glGetInteger64v(pname, &value);
    return value;
  } else {
    static_assert(std::false_type::value, "Unsupported type for glGet");
  }
}

template <typename T>
auto get(GLenum pname, GLuint index) {
  if constexpr (std::same_as<T, GLboolean>) {
    GLboolean value;
    glGetBooleani_v(pname, index, &value);
    return value;
  } else if constexpr (std::same_as<T, GLfloat>) {
    GLfloat value;
    glGetFloati_v(pname, index, &value);
    return value;
  } else if constexpr (std::same_as<T, GLdouble>) {
    GLdouble value;
    glGetDoublei_v(pname, index, &value);
    return value;
  } else if constexpr (std::same_as<T, GLint>) {
    GLint value;
    glGetIntegeri_v(pname, index, &value);
    return value;
  } else if constexpr (std::same_as<T, GLint64>) {
    GLint64 value;
    glGetInteger64i_v(pname, index, &value);
    return value;
  } else {
    static_assert(std::false_type::value, "Unsupported type for glGet");
  }
}

template <int L, typename T>
auto gets(GLenum pname) {
  if constexpr (std::same_as<T, GLboolean>) {
    glm::vec<L, GLboolean> value;
    glGetBooleanv(pname, glm::value_ptr(value));
    return value;
  } else if constexpr (std::same_as<T, GLfloat>) {
    glm::vec<L, GLfloat> value;
    glGetFloatv(pname, glm::value_ptr(value));
    return value;
  } else if constexpr (std::same_as<T, GLdouble>) {
    glm::vec<L, GLdouble> value;
    glGetDoublev(pname, glm::value_ptr(value));
    return value;
  } else if constexpr (std::same_as<T, GLint>) {
    glm::vec<L, GLint> value;
    glGetIntegerv(pname, glm::value_ptr(value));
    return value;
  } else if constexpr (std::same_as<T, GLint64>) {
    glm::vec<L, GLint64> value;
    glGetInteger64v(pname, glm::value_ptr(value));
    return value;
  } else {
    static_assert(std::false_type::value, "Unsupported type for glGet");
  }
}

template <int L, typename T>
auto gets(GLenum pname, GLuint index) {
  if constexpr (std::same_as<T, GLboolean>) {
    glm::vec<L, GLboolean> value;
    glGetBooleani_v(pname, index, glm::value_ptr(value));
    return value;
  } else if constexpr (std::same_as<T, GLfloat>) {
    glm::vec<L, GLfloat> value;
    glGetFloati_v(pname, index, glm::value_ptr(value));
    return value;
  } else if constexpr (std::same_as<T, GLdouble>) {
    glm::vec<L, GLdouble> value;
    glGetDoublei_v(pname, index, glm::value_ptr(value));
    return value;
  } else if constexpr (std::same_as<T, GLint>) {
    glm::vec<L, GLint> value;
    glGetIntegeri_v(pname, index, glm::value_ptr(value));
    return value;
  } else if constexpr (std::same_as<T, GLint64>) {
    glm::vec<L, GLint64> value;
    glGetInteger64i_v(pname, index, glm::value_ptr(value));
    return value;
  } else {
    static_assert(std::false_type::value, "Unsupported type for glGet");
  }
}

} // namespace paimon