#include "src/network/uPnPHdlr.hpp"
#include "src/base/Defaults.hpp"

int main(int argc, char *argv[]) {
    UnderStory::UPnPHandler handler(
        UnderStory::Defaults::UPNP_DEFAULT_TARGET_PORT,
        UnderStory::Defaults::UPNP_REQUEST_DESCRIPTION
    );

    // request redirect
    auto redirectRequest = handler.run();
    auto status = redirectRequest.wait_for(std::chrono::seconds(5));

    // request undirect
    auto undirectRequest = handler.stop();
    status = undirectRequest.wait_for(std::chrono::seconds(5));
}
