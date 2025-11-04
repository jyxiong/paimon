#pragma once

#include "paimon/opengl/base/object.h"

namespace paimon {

class Buffer : public NamedObject {
public:

Buffer();
~Buffer() override;

Buffer(const Buffer &other) = delete;
Buffer &operator=(const Buffer &other) = delete;

Buffer(Buffer &&other) = default;

bool is_valid() const override;

void set_storage(GLsizeiptr size, const void *data, GLbitfield flags = 0) const;

void set_data(GLintptr offset, GLsizeiptr size, const void *data) const;

void set_sub_data(GLintptr offset, GLsizeiptr size, const void *data) const;

void invalidate_data() const;

void invalidate_sub_data(GLintptr offset, GLsizeiptr size) const;

void copy_sub_data(const Buffer &source, GLintptr read_offset, GLintptr write_offset, GLsizeiptr size) const;

void clear_data(GLenum internalformat, GLenum format, GLenum type, const void *data) const;

void map(GLenum access) const;

void *map_range(GLintptr offset, GLsizeiptr length, GLbitfield access) const;

void flush_mapped_range(GLintptr offset, GLsizeiptr length) const;

bool unmap() const;

void bind(GLenum target) const;
static void unbind(GLenum target);

void bind_base(GLenum target, GLuint index) const;

void bind_range(GLenum target, GLuint index, GLintptr offset, GLsizeiptr size) const;

template<typename T>
void get(GLenum pname, T *params) const;

template<typename T>
T get(GLenum pname) const {
  T params;
  get(pname, &params);
  return params;
}

void* get_pointer() const;

void get_sub_data(GLintptr offset, GLsizeiptr size, void *data) const;
};

}