#pragma once

#include <filesystem>

namespace Paimon
{

class Application
{
public:
    static const std::filesystem::path &GetDataPath() { return s_dataPath; }
    static void SetDataPath(const std::filesystem::path &path) { s_dataPath = path; };

public:
    static std::filesystem::path s_dataPath;

}; // class Application

} // namespace Paimon
