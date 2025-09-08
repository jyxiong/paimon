#pragma once

#ifdef __linux__

#include "paimon/platform/context.h"

#include <memory>

#include "glad/glx.h"

#include "paimon/platform/context_format.h"

namespace paimon {
class GlxContext : public Context {
public:
  GlxContext();
  ~GlxContext() override;

  bool destroy() override;

  long long nativeHandle() override;

  bool valid() override;

  bool makeCurrent() override;

  bool doneCurrent() override;

  static std::unique_ptr<Context> getCurrent();

  static std::unique_ptr<Context> create(const ContextFormat &format);

private:
  void createContext(GLXContext shared, const ContextFormat &format);

private:
  GLXDrawable m_drawable;
  GLXPbuffer m_pBuffer;
  GLXContext m_contextHandle;
  bool m_owning;
};
} // namespace paimon

#endif // __linux__
