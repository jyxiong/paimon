#include "paimon/platform/context.h"

#include <cassert>

#include "paimon/core/base/macro.h"

using namespace paimon;

Context::Context() : m_threadId(std::this_thread::get_id()) {}

Context::~Context() {
  if (std::this_thread::get_id() != m_threadId) {
    LOG_ERROR("Context destroyed from wrong thread");
  }
}