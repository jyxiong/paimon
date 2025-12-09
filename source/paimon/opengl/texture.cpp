#include "paimon/opengl/texture.h"
#include <glad/gl.h>
#include "texture.h"

using namespace paimon;

Texture::Texture(GLenum target) : NamedObject(GL_TEXTURE), m_target(target) {
  glCreateTextures(target, 1, &m_name);
}

Texture::~Texture() {
  if (m_name != 0) {
    glDeleteTextures(1, &m_name);
  }
}

bool Texture::is_valid() const { return glIsTexture(m_name) == GL_TRUE; }

void Texture::bind(GLuint unit) const { glBindTextureUnit(unit, m_name); }

void Texture::bind(GLuint unit, GLenum access, GLenum format, GLuint level,
                   GLboolean layered, GLuint layer) const {
  glBindImageTexture(unit, m_name, level, layered, layer, access, format);
}

void Texture::set_buffer_data(GLenum internalformat, GLuint buffer) {
  glTextureBuffer(m_name, internalformat, buffer);
}

void Texture::set_buffer_range(GLenum internalformat, GLuint buffer,
                               GLintptr offset, GLsizeiptr size) {
  glTextureBufferRange(m_name, internalformat, buffer, offset, size);
}

void Texture::set_storage_1d(GLsizei levels, GLenum internalformat,
                             GLsizei width) {
  glTextureStorage1D(m_name, levels, internalformat, width);
}

void Texture::set_storage_2d(GLsizei levels, GLenum internalformat,
                             GLsizei width, GLsizei height) {
  glTextureStorage2D(m_name, levels, internalformat, width, height);
}

void Texture::set_storage_3d(GLsizei levels, GLenum internalformat,
                             GLsizei width, GLsizei height, GLsizei depth) {
  glTextureStorage3D(m_name, levels, internalformat, width, height, depth);
}

void Texture::set_storage_2d_multisample(GLsizei samples, GLenum internalformat,
                                         GLsizei width, GLsizei height,
                                         GLboolean fixedsamplelocations) {
  glTextureStorage2DMultisample(m_name, samples, internalformat, width, height,
                                fixedsamplelocations);
}

void Texture::set_storage_3d_multisample(GLsizei samples, GLenum internalformat,
                                         GLsizei width, GLsizei height,
                                         GLsizei depth,
                                         GLboolean fixedsamplelocations) {
  glTextureStorage3DMultisample(m_name, samples, internalformat, width, height,
                                depth, fixedsamplelocations);
}

void Texture::set_sub_image_1d(GLint level, GLint xoffset, GLsizei width,
                               GLenum format, GLenum type, const void *pixels) {
  glTextureSubImage1D(m_name, level, xoffset, width, format, type, pixels);
}

void Texture::set_sub_image_2d(GLint level, GLint xoffset, GLint yoffset,
                               GLsizei width, GLsizei height, GLenum format,
                               GLenum type, const void *pixels) {
  glTextureSubImage2D(m_name, level, xoffset, yoffset, width, height, format,
                      type, pixels);
}

void Texture::set_sub_image_3d(GLint level, GLint xoffset, GLint yoffset,
                               GLint zoffset, GLsizei width, GLsizei height,
                               GLsizei depth, GLenum format, GLenum type,
                               const void *pixels) {
  glTextureSubImage3D(m_name, level, xoffset, yoffset, zoffset, width, height,
                      depth, format, type, pixels);
}

void Texture::set_compressed_sub_image_1d(GLint level, GLint xoffset,
                                          GLsizei width, GLenum format,
                                          GLsizei image_size,
                                          const void *data) {
  glCompressedTextureSubImage1D(m_name, level, xoffset, width, format,
                                image_size, data);
}

void Texture::set_compressed_sub_image_2d(GLint level, GLint xoffset,
                                          GLint yoffset, GLsizei width,
                                          GLsizei height, GLenum format,
                                          GLsizei image_size,
                                          const void *data) {
  glCompressedTextureSubImage2D(m_name, level, xoffset, yoffset, width, height,
                                format, image_size, data);
}

void Texture::set_compressed_sub_image_3d(GLint level, GLint xoffset,
                                          GLint yoffset, GLint zoffset,
                                          GLsizei width, GLsizei height,
                                          GLsizei depth, GLenum format,
                                          GLsizei image_size,
                                          const void *data) {
  glCompressedTextureSubImage3D(m_name, level, xoffset, yoffset, zoffset, width,
                                height, depth, format, image_size, data);
}

void Texture::copy_sub_image_1d(GLint level, GLint xoffset, GLint x, GLint y,
                                GLsizei width) {
  glCopyTextureSubImage1D(m_name, level, xoffset, x, y, width);
}

void Texture::copy_sub_image_2d(GLint level, GLint xoffset, GLint yoffset,
                                GLint x, GLint y, GLsizei width,
                                GLsizei height) {
  glCopyTextureSubImage2D(m_name, level, xoffset, yoffset, x, y, width, height);
}

void Texture::copy_sub_image_3d(GLint level, GLint xoffset, GLint yoffset,
                                GLint zoffset, GLint x, GLint y, GLsizei width,
                                GLsizei height) {
  glCopyTextureSubImage3D(m_name, level, xoffset, yoffset, zoffset, x, y, width,
                          height);
}

void Texture::generate_mipmap() { glGenerateTextureMipmap(m_name); }

void Texture::get_image(GLint level, GLenum format, GLenum type,
                        GLsizei buf_size, void *pixels) {
  glGetTextureImage(m_name, level, format, type, buf_size, pixels);
}

void Texture::get_compressed_image(GLint level, GLsizei buf_size,
                                   void *pixels) {
  glGetCompressedTextureImage(m_name, level, buf_size, pixels);
}

GLenum Texture::get_target() const { return m_target; }

template <>
void Texture::get(GLenum property, GLint *value) {
  glGetTextureParameteriv(m_name, property, value);
}

template <>
void Texture::get(GLenum property, GLfloat *value) {
  glGetTextureParameterfv(m_name, property, value);
}

template <>
GLint Texture::get(GLenum property) {
  GLint value;
  get(property, &value);
  return value;
}

template <>
GLfloat Texture::get(GLenum property) {
  GLfloat value;
  get(property, &value);
  return value;
}

template <>
void Texture::get(GLint level, GLenum property, GLint *value) {
  glGetTextureLevelParameteriv(m_name, level, property, value);
}

template <>
void Texture::get(GLint level, GLenum property, GLfloat *value) {
  glGetTextureLevelParameterfv(m_name, level, property, value);
}

template <>
GLint Texture::get(GLint level, GLenum property) {
  GLint value;
  get(level, property, &value);
  return value;
}

template <>
GLfloat Texture::get(GLint level, GLenum property) {
  GLfloat value;
  get(level, property, &value);
  return value;
}

template <>
void Texture::set<GLint>(GLenum property, GLint value) {
  glTextureParameteri(m_name, property, value);
}

template <>
void Texture::set<GLenum>(GLenum property, GLenum value) {
  glTextureParameteri(m_name, property, static_cast<GLint>(value));
}

template <>
void Texture::set<GLfloat>(GLenum property, GLfloat value) {
  glTextureParameterf(m_name, property, value);
}

template <>
void Texture::set<GLint *>(GLenum property, GLint *value) {
  glTextureParameteriv(m_name, property, value);
}

template <>
void Texture::set<GLfloat *>(GLenum property, GLfloat *value) {
  glTextureParameterfv(m_name, property, value);
}

template <>
void Texture::set<const GLint *>(GLenum property, const GLint *value) {
  glTextureParameteriv(m_name, property, value);
}

template <>
void Texture::set<const GLfloat *>(GLenum property, const GLfloat *value) {
  glTextureParameterfv(m_name, property, value);
}
