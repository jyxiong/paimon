#pragma once

#include <chrono>

namespace Paimon
{
class StopWatch
{
public:
    StopWatch() = default;
    ~StopWatch() = default;

    void Start() { m_beginTime = std::chrono::system_clock::now(); }
    void Stop() { m_endTime = std::chrono::system_clock::now(); }

    std::int64_t GetNanoseconds()
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(m_endTime - m_beginTime).count();
    }

    std::int64_t GetMicroseconds()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(m_endTime - m_beginTime).count();
    }

    std::int64_t GetMilliseconds()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(m_endTime - m_beginTime).count();
    }

    std::int64_t GetSeconds()
    {
        return std::chrono::duration_cast<std::chrono::seconds>(m_endTime - m_beginTime).count();
    }

private:
    std::chrono::system_clock::time_point m_beginTime;
    std::chrono::system_clock::time_point m_endTime;

}; // class StopWatch

} // namespace Paimon

