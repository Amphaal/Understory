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

#include <chrono>

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#include "src/network/uPnPHdlr.hpp"

#include "src/network/USServer.hpp"
#include "src/network/USClient.hpp"

//
// Test cases
//

/* TEST_CASE("Test uPnP", "[network]") {
    UnderStory::UPnPHandler handler(
        UnderStory::Defaults::UPNP_DEFAULT_TARGET_PORT,
        UnderStory::Defaults::UPNP_REQUEST_DESCRIPTION
    );

    // request redirect
    auto redirectRequest = handler.run();
    auto status = redirectRequest.wait_for(std::chrono::seconds(5));
    REQUIRE(status != std::future_status::timeout);
    REQUIRE(handler.hasSucceded());

    // request undirect
    auto undirectRequest = handler.stop();
    status = undirectRequest.wait_for(std::chrono::seconds(5));
    REQUIRE(status != std::future_status::timeout);
    REQUIRE(undirectRequest.get() == 0);
} */

using UnderStory::Network::Server::USServer;
using UnderStory::Network::USClient;

TEST_CASE("client / server sample", "[network]") {
    spdlog::set_level(spdlog::level::debug);

    // start server
    asio::io_context serverContext;
    USServer server(serverContext);
    std::thread serverThread([&serverContext](){ serverContext.run(); });

    // start client
    asio::io_context clientContext;
    USClient client1(clientContext, "127.0.0.1");
    std::thread clientThread([&clientContext](){ clientContext.run(); });

    // define handshake
    client1.connectAs("TestUser");

    // send
    // client1.sendHandshake(hsIn);

    // // check raw payload
    // auto payload = server.waitForRawPayload();
    // REQUIRE(payload.type == UnderStory::PayloadType::HANDSHAKE);

    // // parse
    // Handshake hsOut;
    // hsOut.ParseFromString(payload.bytes);

    // // check payload content
    // REQUIRE(hsOut.client_version() == *currentVersion);
    // REQUIRE(hsOut.username() == *username);

    serverThread.join();
    clientThread.join();
}
