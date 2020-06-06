#include <future>

void _isNewerVersionAvailable() {
    auto i = true;
}

int main(int argc, char *argv[]) {
    auto q = std::async(
            _isNewerVersionAvailable
        );
    return 0;
}