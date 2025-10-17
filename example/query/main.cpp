#include <glm/glm.hpp>

#include "paimon/core/log_system.h"
#include "paimon/opengl/query.h"
#include "paimon/platform/window.h"

using namespace paimon;

namespace {
glm::ivec2 g_size = {800, 600};
} // namespace

int main() {
  LogSystem::init();

  auto window = Window::create(WindowConfig{
      .title = "Query Example",
      .format =
          ContextFormat{
              .majorVersion = 4,
              .minorVersion = 5,
              .profile = ContextProfile::Core,
          },
      .width = static_cast<uint32_t>(g_size.x),
      .height = static_cast<uint32_t>(g_size.y),
  });

  Query query(GL_TIME_ELAPSED);
  if (!query.is_valid()) {
    LOG_ERROR("Failed to create query object");
    return -1;
  }

  // 演示一次 OpenGL 查询过程
  query.begin();
  // 执行一段 OpenGL 操作，比如清屏
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  query.end();

  // 获取查询结果（阻塞直到结果可用）
  GLuint64 elapsed = query.get<GLuint64>(GL_QUERY_RESULT);
  LOG_INFO("GL_TIME_ELAPSED: {} ns", elapsed);

  window->destroy();

  return 0;
}