#include <catch2/catch.hpp>

#include "src/core/UpdateChecker.hpp"
using UnderStory::UpdateChecker_Private;

TEST_CASE("Download update manifest", "[update checker]") {
    spdlog::set_level(spdlog::level::debug);

    auto manifest = UpdateChecker_Private::_getRemoteManifestContent();
    REQUIRE(!manifest.empty());

    auto versions = UpdateChecker_Private::_extractVersionsFromManifest(manifest);
    spdlog::debug("Compared Versions : Local [{}] <> Remote [{}]", versions["App"], APP_CURRENT_VERSION);
    REQUIRE(!version.empty());
}
