#include "test_framework.h"

#include "engine/tools/command_registry.h"

using namespace engine::tools;

TEST_CASE("commands: tokenize handles quotes") {
    auto t = CommandRegistry::tokenize(R"(spawn  "big enemy" 3 "")");
    CHECK(t.size() == 4);
    CHECK(t[0] == "spawn");
    CHECK(t[1] == "big enemy");
    CHECK(t[2] == "3");
    CHECK(t[3].empty());
}

TEST_CASE("commands: register and execute") {
    CommandRegistry reg;
    int total = 0;
    CHECK(reg.add("add", "sum numbers", [&](const CommandArgs& args) {
        for (const auto& a : args) {
            total += std::stoi(a);
        }
        return CommandResult::success(std::to_string(total));
    }));
    CHECK(!reg.add("add", "dup", [](const CommandArgs&) { return CommandResult::success(); }));

    auto r = reg.execute("add 1 2 3");
    CHECK(r.ok);
    CHECK(r.output == "6");

    CHECK(reg.execute("echo hi there").output == "hi there");
    CHECK(!reg.execute("nope").ok);
    CHECK(!reg.execute("   ").ok);
    CHECK(reg.execute("help").output.find("add - sum numbers") != std::string::npos);
}
