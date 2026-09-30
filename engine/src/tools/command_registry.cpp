#include "engine/tools/command_registry.h"

#include <cctype>

namespace engine::tools {

CommandRegistry::CommandRegistry() {
    add("help", "列出所有命令", [this](const CommandArgs&) {
        std::string out;
        for (const CommandInfo* info : list()) {
            out += info->name + " - " + info->help + "\n";
        }
        return CommandResult::success(std::move(out));
    });
    add("echo", "原样输出参数", [](const CommandArgs& args) {
        std::string out;
        for (size_t i = 0; i < args.size(); ++i) {
            out += (i ? " " : "") + args[i];
        }
        return CommandResult::success(std::move(out));
    });
}

bool CommandRegistry::add(std::string name, std::string help, CommandFn fn) {
    if (name.empty() || !fn || m_commands.contains(name)) {
        return false;
    }
    CommandInfo info{name, std::move(help), std::move(fn)};
    m_commands.emplace(std::move(name), std::move(info));
    return true;
}

bool CommandRegistry::contains(std::string_view name) const {
    return m_commands.find(name) != m_commands.end();
}

CommandResult CommandRegistry::execute(std::string_view line) const {
    CommandArgs tokens = tokenize(line);
    if (tokens.empty()) {
        return CommandResult::failure("empty command");
    }
    auto it = m_commands.find(tokens.front());
    if (it == m_commands.end()) {
        return CommandResult::failure("unknown command: " + tokens.front());
    }
    CommandArgs args(tokens.begin() + 1, tokens.end());
    return it->second.fn(args);
}

std::vector<const CommandInfo*> CommandRegistry::list() const {
    std::vector<const CommandInfo*> out;
    out.reserve(m_commands.size());
    for (const auto& [name, info] : m_commands) {
        out.push_back(&info);
    }
    return out;
}

CommandArgs CommandRegistry::tokenize(std::string_view line) {
    CommandArgs tokens;
    std::string current;
    bool inQuotes = false;
    bool hasToken = false;
    for (char c : line) {
        if (c == '"') {
            inQuotes = !inQuotes;
            hasToken = true;
        } else if (!inQuotes && std::isspace(static_cast<unsigned char>(c))) {
            if (hasToken) {
                tokens.push_back(std::move(current));
                current.clear();
                hasToken = false;
            }
        } else {
            current += c;
            hasToken = true;
        }
    }
    if (hasToken) {
        tokens.push_back(std::move(current));
    }
    return tokens;
}

} // namespace engine::tools
