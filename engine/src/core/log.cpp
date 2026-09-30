#include "engine/core/log.h"

#include <cstdio>
#include <mutex>

namespace engine::log {
namespace {

std::mutex g_mutex;
Level g_minLevel = Level::Info;

void defaultSink(Level level, std::string_view channel, std::string_view message) {
    std::FILE* out = level >= Level::Warn ? stderr : stdout;
    std::fprintf(out, "[%s][%.*s] %.*s\n", levelName(level), static_cast<int>(channel.size()),
                 channel.data(), static_cast<int>(message.size()), message.data());
}

Sink g_sink = defaultSink;

} // namespace

void setSink(Sink sink) {
    std::lock_guard lock(g_mutex);
    g_sink = sink ? std::move(sink) : Sink(defaultSink);
}

void resetSink() { setSink(nullptr); }

void setMinLevel(Level level) {
    std::lock_guard lock(g_mutex);
    g_minLevel = level;
}

void write(Level level, std::string_view channel, std::string_view message) {
    std::lock_guard lock(g_mutex);
    if (level < g_minLevel) {
        return;
    }
    g_sink(level, channel, message);
}

const char* levelName(Level level) {
    switch (level) {
    case Level::Trace:
        return "TRACE";
    case Level::Info:
        return "INFO";
    case Level::Warn:
        return "WARN";
    case Level::Error:
        return "ERROR";
    }
    return "?";
}

} // namespace engine::log
