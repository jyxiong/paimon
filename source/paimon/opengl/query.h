#pragma once

#include "paimon/opengl/base/object.h"

namespace paimon {

class Query : public NamedObject {
public:
  Query(GLenum type);
  ~Query();

  Query(const Query &other) = delete;
  Query &operator=(const Query &other) = delete;

  Query(Query &&other) = default;

  bool is_valid() const override;

public:
  GLenum get_type() const;

  void begin();

  void end();

  template <class T> void get(GLenum property, T *value);

  template <class T> T get(GLenum property);

private:
  const GLenum m_type;
};

} // namespace paimon