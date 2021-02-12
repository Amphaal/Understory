#include <catch2/catch.hpp>

#include "src/network/Server.h"
#include "src/network/ClientSocket.h"

#include "src/base/Defaults.hpp"

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
