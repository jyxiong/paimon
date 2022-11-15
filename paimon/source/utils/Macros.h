#pragma once

#include "Log.h"

namespace Paimon
{

// core log
#define CORE_LOG_TRACE(...) Log::GetCoreLogger()->trace(__VA_ARGS__);
#define CORE_LOG_INFO(...) Log::GetCoreLogger()->info(__VA_ARGS__);
#define CORE_LOG_WARN(...) Log::GetCoreLogger()->warn(__VA_ARGS__);
#define CORE_LOG_ERROR(...) Log::GetCoreLogger()->error(__VA_ARGS__);
#define CORE_LOG_FATAL(...) Log::GetCoreLogger()->fetal(__VA_ARGS__);

// client log
#define LOG_TRACE(...) Log::GetClientLogger()->trace(__VA_ARGS__);
#define LOG_INFO(...) Log::GetClientLogger()->info(__VA_ARGS__);
#define LOG_WARN(...) Log::GetClientLogger()->warn(__VA_ARGS__);
#define LOG_ERROR(...) Log::GetClientLogger()->error(__VA_ARGS__);
#define LOG_FATAL(...) Log::GetClientLogger()->fetal(__VA_ARGS__);

// gl error check
#define CHECK_GL_ERROR \
{ \
    auto errorCode = glGetError(); \
    if (errorCode != GL_NO_ERROR)  \
    {                        \
        CORE_LOG_ERROR("OpenGL Error {}", errorCode);                        \
    }\
};

} // namespace Paimon
