#pragma once

#include <memory>

#include "spdlog/spdlog.h"

namespace Paimon
{

class Log
{
public:
    Log() = default;
    ~Log() = default;

    static void Init();

    inline static std::shared_ptr<spdlog::logger> GetCoreLogger() { return s_coreLogger; }
    inline static std::shared_ptr<spdlog::logger> GetClientLogger() { return s_clientLogger; }

private:
    static std::shared_ptr<spdlog::logger> s_coreLogger;
    static std::shared_ptr<spdlog::logger> s_clientLogger;

}; // class Log

} // namespace Paimon
