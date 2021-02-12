#include <catch2/catch.hpp>

#include "src/core/UpdateChecker.hpp"
using UnderStory::UpdateChecker_Private;

TEST_CASE("Version comparaison", "[update checker]") {
    spdlog::set_level(spdlog::level::debug);

    std::string test_localVersion("0.5.0");
    auto testUpdateChecker = [test_localVersion](const std::string &remoteVersion) {
        return UpdateChecker_Private::_isRemoteVersionNewerThanLocal(remoteVersion, test_localVersion);
    };

    REQUIRE(testUpdateChecker("0.5.1"));
    REQUIRE(testUpdateChecker("0.5.10"));
    REQUIRE(testUpdateChecker("1.0"));

    REQUIRE_FALSE(testUpdateChecker(APP_CURRENT_VERSION));
    REQUIRE_FALSE(testUpdateChecker("0.5.0"));
    REQUIRE_FALSE(testUpdateChecker("0.4.10"));
}
