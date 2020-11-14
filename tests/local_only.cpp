// UnderStory
// An intuitive Pen & Paper experience
// Copyright (C) 2020 Guillaume Vara

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Any graphical resources available within the source code may
// use a different license and copyright : please refer to their metadata
// for further details. Graphical resources without explicit references to a
// different license and copyright still refer to this GPL.

#include <spdlog/spdlog.h>

#include <chrono>
#include <future>

#include "src/core/UpdateChecker.hpp"
using UnderStory::UpdateChecker_Private;

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

// #include "src/network/USServer.hpp"
// #include "src/network/USClient.hpp"

//
// Test cases
//

/* using UnderStory::Network::Server::USServer;
using UnderStory::Network::USClient;
using UnderStory::Network::RawPayload;
using UnderStory::Network::PayloadType;
using UnderStory::Defaults;

TEST_CASE("client / server - Handshake", "[network]") {
    spdlog::set_level(spdlog::level::debug);

    // app Context
    auto appContext = UnderStory::Context::random();

    // start server
    asio::io_context serverContext;
    USServer server(appContext, serverContext);
    std::thread serverThread([&serverContext](){ serverContext.run(); });

    // start client
    asio::io_context clientContext;
    USClient client1(clientContext, "127.0.0.1");
    std::thread clientThread([&clientContext](){ clientContext.run(); });

    // define env
    auto username = "TestUser";

    // define test
    server.callbacks.onPayloadReceived = [username, &serverContext, &clientContext](const RawPayload & payload) {
        // check type
        REQUIRE(payload.type == PayloadType::HANDSHAKE);

        // parse
        Handshake hsOut;
        hsOut.ParseFromString(payload.bytes);

        // check payload content
        REQUIRE(hsOut.client_version() == APP_CURRENT_VERSION);
        REQUIRE(hsOut.username() == username);

        // stop contexts
        serverContext.stop();
        clientContext.stop();
    };

    // send handshake to server
    client1.connectAs(username);

    // wait for threads to finish
    serverThread.join();
    clientThread.join();
} */

TEST_CASE("Download update manifest", "[update checker]") {
    spdlog::set_level(spdlog::level::debug);

    auto manifest = UpdateChecker_Private::_getManifest();
    REQUIRE(!manifest.empty());

    auto version = UpdateChecker_Private::_extractRemoteVersionFromManifest(manifest);
    spdlog::debug("Compared Versions : Local [{}] <> Remote [{}]", version, APP_CURRENT_VERSION);
    REQUIRE(!version.empty());
}
