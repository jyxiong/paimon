#pragma once

#include <memory>

#include "spdlog/spdlog.h"

namespace paimon {

class LogSystem {
public:
  static void init();
  static std::shared_ptr<spdlog::logger> &getLogger();

private:
  static std::shared_ptr<spdlog::logger> sLogger;
};

} // namespace paimon

#define LOG_INFO(...) paimon::LogSystem::getLogger()->info(__VA_ARGS__)
#define LOG_ERROR(...) paimon::LogSystem::getLogger()->error(__VA_ARGS__)
#define LOG_WARN(...) paimon::LogSystem::getLogger()->warn(__VA_ARGS__)
#define LOG_DEBUG(...) paimon::LogSystem::getLogger()->debug(__VA_ARGS__)