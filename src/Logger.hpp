#pragma once

#include <atomic>
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <mutex>

namespace itsamonster {

    // Simple thread-safe singleton logger with optional verbosity control.
    // IS NOT PROPERLY IMPLEMENTED YET, just a placeholder for future work.
    class Logger {
    public:
        static Logger& Instance() {
            static Logger instance; // guaranteed thread-safe in C++11+
            return instance;
        }

        void SetVerbose(bool v) noexcept { m_verbose.store(v, std::memory_order_relaxed); }
        bool IsVerbose() const noexcept { return m_verbose.load(std::memory_order_relaxed); }

        void Debug(const char* fmt, ...);

        void LogMonster(struct Monster& monster, const char* fmt, ...);

    private:
        Logger() = default;
        ~Logger() = default;
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
        Logger(Logger&&) = delete;
        Logger& operator=(Logger&&) = delete;

        void vprint(const char* level, const char* fmt, va_list args);
    private:
        std::atomic<bool> m_verbose{ false };
        std::mutex m_mutex; // guard output to avoid interleaving
    };

    // Macro preserves existing call style: LOG("text " << value);
#define LOG(msg) do { \
    if (Logger::Instance().IsVerbose()) { \
        std::cout << msg << std::endl; \
    } \
} while(0)

#define LOG_ERROR(msg) do { \
    if (Logger::Instance().IsVerbose()) { \
        std::cout << msg << std::endl; \
    } \
} while(0)

#define LOGGER Logger::Instance()
} // namespace itsamonster