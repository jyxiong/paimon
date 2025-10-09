#pragma once

namespace paimon {
class GLFWPlatform {
public:
  static GLFWPlatform &instance();

  ~GLFWPlatform();
private:
  GLFWPlatform();
};
}