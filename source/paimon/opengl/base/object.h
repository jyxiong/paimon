#pragma once

#include <string>

#include "glad/gl.h"

namespace paimon {
class Object {
public:
  Object() = default;

  virtual ~Object() = default;

  Object(const Object &other) = delete;
  Object &operator=(const Object &other) = delete;

  Object(Object &&other) noexcept = default;

public:
  virtual bool is_valid() const = 0;
  virtual std::string get_label() const = 0;
  virtual void set_label(const std::string &label) = 0;
};

class NamedObject : public Object {

public:
  NamedObject(GLenum identifier);

  virtual ~NamedObject() = default;

  NamedObject(const NamedObject &other) = delete;
  NamedObject &operator=(const NamedObject &other) = delete;

  NamedObject(NamedObject &&other) noexcept;

  std::string get_label() const override;
  void set_label(const std::string &label) override;

  GLuint get_name() const;

protected:
  GLenum m_identifier;
  GLuint m_name;
};

class SyncObject : public Object {
public:
  SyncObject() = default;

  virtual ~SyncObject() = default;

  SyncObject(const SyncObject &other) = delete;
  SyncObject &operator=(const SyncObject &other) = delete;

  SyncObject(SyncObject &&other) noexcept;

  std::string get_label() const override;
  void set_label(const std::string &label) override;

private:
  GLsync m_sync;
};

} // namespace paimon
