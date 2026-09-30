#include "test_framework.h"

#include <cstring>

// 用法：engine_tests [名称子串]   —— 只运行名称包含该子串的用例
int main(int argc, char** argv) {
    const char* filter = argc > 1 ? argv[1] : nullptr;
    int run = 0;
    int failedCases = 0;
    for (const auto& c : test::registry()) {
        if (filter && !std::strstr(c.name, filter)) {
            continue;
        }
        const int before = test::failures();
        c.fn();
        ++run;
        const bool ok = test::failures() == before;
        failedCases += ok ? 0 : 1;
        std::printf("[%s] %s\n", ok ? " OK " : "FAIL", c.name);
    }
    std::printf("\n%d test(s) run, %d failed\n", run, failedCases);
    return failedCases == 0 ? 0 : 1;
}
