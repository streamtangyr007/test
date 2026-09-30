// 命令注册表（开发者控制台）—— 对应《游戏引擎架构》第10章（调试与开发工具）。
//
// 这是引擎"对 AI 友好"的关键入口：
//   - 人类在游戏内控制台输入 `spawn enemy 3`
//   - 自动化测试脚本逐行执行命令
//   - 将来 AI 代理（例如通过 MCP 服务器）调用同一批命令来查询/操作引擎状态
// 所有命令都返回结构化的 CommandResult，而不是只打印到屏幕。
#pragma once

#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace engine::tools {

struct CommandResult {
    bool ok = true;
    std::string output;

    static CommandResult success(std::string out = {}) { return {true, std::move(out)}; }
    static CommandResult failure(std::string out) { return {false, std::move(out)}; }
};

using CommandArgs = std::vector<std::string>;
using CommandFn = std::function<CommandResult(const CommandArgs&)>;

struct CommandInfo {
    std::string name;
    std::string help;
    CommandFn fn;
};

class CommandRegistry {
public:
    CommandRegistry();

    // 重名注册返回 false。
    bool add(std::string name, std::string help, CommandFn fn);
    bool contains(std::string_view name) const;

    // 执行一行命令，例如 `echo hello "big world"`。支持双引号包裹含空格的参数。
    CommandResult execute(std::string_view line) const;

    // 按名称排序的命令列表（给 `help` 命令和 AI 工具发现可用命令用）。
    std::vector<const CommandInfo*> list() const;

    static CommandArgs tokenize(std::string_view line);

private:
    std::map<std::string, CommandInfo, std::less<>> m_commands;
};

} // namespace engine::tools
