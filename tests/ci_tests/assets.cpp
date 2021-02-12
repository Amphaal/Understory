#include <catch2/catch.hpp>

#include "src/core/AssetIntegrator.hpp"

TEST_CASE("Test asset integration", "[assets]") {
    spdlog::set_level(spdlog::level::debug);

    // create package
    auto path = fs::absolute("./tests/resources/test.png");
    auto asset = UnderStory::AssetIntegrator::createAsset(path);
    auto hash = asset.file().file_description().fnv1a_hash();

    REQUIRE(asset.file().file_description().name() == "test");
    REQUIRE(asset.file().file_description().file_extension() == ".png");
    REQUIRE(hash == 9008380550126030887);
    REQUIRE(asset.file().content_length() == 25579);
    REQUIRE(asset.dimensions().width() == 290);
    REQUIRE(asset.dimensions().height() == 300);

    // integrate package to context
    auto context = UnderStory::Context::random();
    UnderStory::AssetContext assetContext(context);
    assetContext.integrateAsset(asset);

    // get package back from context
    auto assetFromContext = assetContext.fetchAsset(hash);
    REQUIRE(assetFromContext.file().content_length() == 25579);
}
