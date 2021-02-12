// UnderStory
// An intuitive Pen & Paper experience
// Copyright (C) 2020-2021 Guillaume Vara <guillaume.vara@gmail.com>

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

#include "src/network/Server.h"
#include "src/network/ClientSocket.h"

#include "src/base/Defaults.hpp"

//
// Test cases
//

using UnderStory::Network::Server;
using UnderStory::Network::ClientSocket;
using UnderStory::Network::SpawnedRawPayload;
using UnderStory::Network::PayloadType;
using UnderStory::Defaults;

TEST_CASE("client / server - Handshake", "[network]") {
    spdlog::set_level(spdlog::level::debug);

    // app Context
    auto appContext = UnderStory::Context::random();

    // start server
    asio::io_context serverContext;
    Server server(
        appContext, 
        serverContext, 
        "Server", 
        Defaults::UPNP_DEFAULT_TARGET_PORT
    );
    std::thread serverThread([&serverContext](){ 
        serverContext.run(); 
    });

    // start client
    asio::io_context clientContext;
    ClientSocket client1(
        clientContext, 
        "Client1", 
        "127.0.0.1", 
        Defaults::UPNP_DEFAULT_TARGET_PORT
    );
    std::thread clientThread([&clientContext](){ 
        clientContext.run(); 
    });

    // define env
    auto username = "TestUser";

    // send handshake to server
    client1.initiateHandshake(username);

    // test callback on main thread
    auto onPayloadReceived = [username, &serverContext, &clientContext](const SpawnedRawPayload & payload) -> bool {        
        // check type
        REQUIRE(payload.type == PayloadType::HANDSHAKE);

        // parse
        Handshake hsOut;
        hsOut.ParseFromString(payload.bytes);

        // check payload content
        REQUIRE(hsOut.client_version() == APP_CURRENT_VERSION);
        REQUIRE(hsOut.username() == username);

        // break processing
        return true;
    };

    // bind callback
    server.processPayloadQueue(onPayloadReceived);

    // stop contexts
    serverContext.stop();
    clientContext.stop();

    // wait for threads to finish
    serverThread.join();
    clientThread.join();
}

// TEST_CASE("Download update manifest", "[update checker]") {
//     spdlog::set_level(spdlog::level::debug);

//     auto manifest = UpdateChecker_Private::_getRemoteManifestContent();
//     REQUIRE(!manifest.empty());

//     auto versions = UpdateChecker_Private::_extractVersionsFromManifest(manifest);
//     spdlog::debug("Compared Versions : Local [{}] <> Remote [{}]", versions["App"], APP_CURRENT_VERSION);
//     REQUIRE(!version.empty());
// }
