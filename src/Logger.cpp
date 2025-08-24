#include "Logger.hpp"

#include <vector>
#include "monsters/Monster.hpp"

using namespace itsamonster;

void Logger::Debug(const char* fmt, ...) {
    if (!IsVerbose()) return;
    va_list args;
    va_start(args, fmt);
    vprint("DEBUG", fmt, args);
    va_end(args);
}

// printf-style monster log, only prints when verbose is enabled
void Logger::LogMonster(const struct Monster& monster, const char* fmt, ...) {
    if (!IsVerbose()) return;
    va_list args;
    va_start(args, fmt);
    std::string name(monster.GetName());
    name += "@" + std::to_string(monster.GetInstanceId());
    vprint(name.c_str(), fmt, args);
    va_end(args);
}

// Internal helper to format and print with a level prefix
void Logger::vprint(const char* level, const char* fmt, va_list args) {
    // Copy args since vsnprintf will consume it if used multiple times
    va_list args_copy;
    va_copy(args_copy, args);

    // Determine required size
    int required = std::vsnprintf(nullptr, 0, fmt, args_copy);
    va_end(args_copy);
    if (required < 0) {
        // Fallback on formatting error
        std::lock_guard<std::mutex> lock(m_mutex);
        std::cout << "[" << level << "] formatting error" << std::endl;
        return;
    }

    std::vector<char> buffer(static_cast<size_t>(required) + 1);
    std::vsnprintf(buffer.data(), buffer.size(), fmt, args);

    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "[" << level << "] " << buffer.data() << std::endl;
}