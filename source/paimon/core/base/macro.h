#pragma once

#include "paimon/core/log/log_system.h"

#define LOG_INFO(...) paimon::LogSystem::getLogger()->info(__VA_ARGS__)
#define LOG_ERROR(...) paimon::LogSystem::getLogger()->error(__VA_ARGS__)
#define LOG_WARN(...) paimon::LogSystem::getLogger()->warn(__VA_ARGS__)
#define LOG_DEBUG(...) paimon::LogSystem::getLogger()->debug(__VA_ARGS__)