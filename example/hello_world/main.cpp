#include "paimon/core/base/macro.h"

using namespace paimon;

int main() {
  LogSystem::init();
  LOG_INFO("Hello world!");

  return 0;
}