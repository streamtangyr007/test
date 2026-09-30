// 日志系统 —— 对应《游戏引擎架构》第10章（调试工具）：分级 + 分频道输出。
// 设计要点：日志可被重定向（sink），方便测试和 AI 工具读取结构化输出。
#pragma once

#include <functional>
#include <string_view>

namespace engine::log {

enum class Level { Trace, Info, Warn, Error };

// sink 接收 (级别, 频道, 消息)。默认输出到 stdout/stderr。
using Sink = std::function<void(Level, std::string_view channel, std::string_view message)>;

void setSink(Sink sink);
void resetSink();
void setMinLevel(Level level);
void write(Level level, std::string_view channel, std::string_view message);

const char* levelName(Level level);

inline void trace(std::string_view ch, std::string_view msg) { write(Level::Trace, ch, msg); }
inline void info(std::string_view ch, std::string_view msg) { write(Level::Info, ch, msg); }
inline void warn(std::string_view ch, std::string_view msg) { write(Level::Warn, ch, msg); }
inline void error(std::string_view ch, std::string_view msg) { write(Level::Error, ch, msg); }

} // namespace engine::log
