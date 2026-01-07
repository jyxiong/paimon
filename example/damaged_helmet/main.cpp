#include "paimon/app/application.h"
#include "paimon/core/log_system.h"

using namespace paimon;

int main() {
  LogSystem::init();

  Application app;
  app.run();

  return 0;
}
