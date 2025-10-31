#pragma once

#include "paimon/opengl/base/object.h"

namespace paimon {
class Texture : public NamedObject {
public:
  Texture(GLenum target);
  ~Texture();

  Texture(const Texture &other) = delete;
  Texture &operator=(const Texture &other) = delete;

  Texture(Texture &&other) = default;

  bool is_valid() const override final;

  void bind(GLuint unit) const;

  void set_buffer_data(GLenum internalformat, GLuint buffer);

  void set_buffer_range(GLenum internalformat, GLuint buffer, GLintptr offset,
                        GLsizeiptr size);

  void set_storage_1d(GLsizei levels, GLenum internalformat, GLsizei width);

  void set_storage_2d(GLsizei levels, GLenum internalformat, GLsizei width,
                      GLsizei height);

  void set_storage_3d(GLsizei levels, GLenum internalformat, GLsizei width,
                      GLsizei height, GLsizei depth);

  void set_storage_2d_multisample(GLsizei samples, GLenum internalformat,
                                  GLsizei width, GLsizei height,
                                  GLboolean fixedsamplelocations);

  void set_storage_3d_multisample(GLsizei samples, GLenum internalformat,
                                  GLsizei width, GLsizei height, GLsizei depth,
                                  GLboolean fixedsamplelocations);

  void set_sub_image_1d(GLint level, GLint xoffset, GLsizei width, GLenum format,
                        GLenum type, const void *pixels);

  void set_sub_image_2d(GLint level, GLint xoffset, GLint yoffset, GLsizei width,
                        GLsizei height, GLenum format, GLenum type,
                        const void *pixels);

  void set_sub_image_3d(GLint level, GLint xoffset, GLint yoffset, GLint zoffset,
                        GLsizei width, GLsizei height, GLsizei depth,
                        GLenum format, GLenum type, const void *pixels);

  void set_compressed_sub_image_1d(GLint level, GLint xoffset, GLsizei width,
                                   GLenum format, GLsizei image_size,
                                   const void *data);

  void set_compressed_sub_image_2d(GLint level, GLint xoffset, GLint yoffset,
                                   GLsizei width, GLsizei height, GLenum format,
                                   GLsizei image_size, const void *data);

  void set_compressed_sub_image_3d(GLint level, GLint xoffset, GLint yoffset,
                                   GLint zoffset, GLsizei width, GLsizei height,
                                   GLsizei depth, GLenum format, GLsizei image_size,
                                   const void *data);

  void copy_sub_image_1d(GLint level, GLint xoffset, GLint x, GLint y,
                         GLsizei width);

  void copy_sub_image_2d(GLint level, GLint xoffset, GLint yoffset, GLint x,
                         GLint y, GLsizei width, GLsizei height);

  void copy_sub_image_3d(GLint level, GLint xoffset, GLint yoffset, GLint zoffset,
                         GLint x, GLint y, GLsizei width, GLsizei height);

  void generate_mipmap();

  void get_image(GLint level, GLenum format, GLenum type, GLsizei buf_size,
                 void *pixels);

  void get_compressed_image(GLint level, GLsizei buf_size, void *pixels);

  GLenum get_target() const;

  template <class T> void get(GLenum property, T *value);

  template <class T> T get(GLenum property);

  template <class T> void get(GLint level, GLenum property, T *value);

  template <class T> T get(GLint level, GLenum property);

  template <class T> void set(GLenum property, T value);

private:
  const GLenum m_target;
};
} // namespace paimon
