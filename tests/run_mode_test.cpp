#include "../source/data/runMode.h"

#include <iostream>

int main() {
    if (RunMode::resolve("normal", false, false) != "superposition" ||
        RunMode::resolve("superposition", false, false) != "superposition" ||
        RunMode::resolve("normal", true, true) != "superposition" ||
        RunMode::resolve("normal", false, true) != "normal") {
        std::cerr << "Run-mode safeguard produced an invalid mode\n";
        return 1;
    }

    std::cout << "Passed run-mode safeguard test\n";
    return 0;
}