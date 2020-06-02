// no copyright

#include <cstdlib>
#include <string>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <vector>

int main(int argc, char *argv[]) {
    std::string cmd = "wine /mingw64/bin/makensis.exe";

    // increment
    std::vector<std::string> arguments(argv + 1, argv + argc);
    arguments.erase(arguments.begin());
    for(auto &arg : arguments) {
        cmd += " ";
        cmd += arg;
    }

    // open pipe
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    // get result
    std::array<char, 128> buffer;
    std::string result;
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    // output it
    std::cout << result;

    return 0;
}
