#pragma once

#if defined _WIN32 || defined WIN32 || defined __WIN32__
#define PAIMON_OS_WINDOWS
#endif

#if defined linux || defined __linux || defined __linux__
#define PAIMON_OS_LINUX
#endif
