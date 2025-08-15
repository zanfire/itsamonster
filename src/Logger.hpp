#pragma once

#include <iostream>
#include <mutex>
#include <sstream>

// Simple thread-safe singleton logger with optional verbosity control.
// IS NOT PROPERLY IMPLEMENTED YET, just a placeholder for future work.
class Logger {
public:
    static Logger &Instance() {
        static Logger instance; // guaranteed thread-safe in C++11+
        return instance;
    }

    void SetVerbose(bool v) noexcept { m_verbose.store(v, std::memory_order_relaxed); }
    bool IsVerbose() const noexcept { return m_verbose.load(std::memory_order_relaxed); }

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    std::atomic<bool> m_verbose{false};
};

// Macro preserves existing call style: LOG("text " << value);
#define LOG(msg) do { \
    if (Logger::Instance().IsVerbose()) { \
        std::cout << msg << std::endl; \
    } \
} while(0)