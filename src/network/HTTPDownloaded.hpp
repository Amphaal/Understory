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

#pragma once

#include <fstream>
#include <string>
#include <asio.hpp>

using asio::ip::tcp;

void GetFile(const std::string& serverName, const std::string& getCommand, std::ofstream& outFile) {
    asio::io_service io_service;

    // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(serverName, "http");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;

    // Try each endpoint until we successfully establish a connection.
    tcp::socket socket(io_service);
    asio::error_code error;
    do {
        socket.close();
        socket.connect(*endpoint_iterator++, error);
    } while (error.value() && endpoint_iterator != end);

    asio::streambuf request;
    std::ostream request_stream(&request);

    request_stream << "GET " << getCommand << " HTTP/1.0\r\n";
    request_stream << "Host: " << serverName << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";

    // Send the request.
    asio::write(socket, request);

    // Read the response status line.
    asio::streambuf response;
    asio::read_until(socket, response, "\r\n");

    // Check that response is OK.
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);


    // Read the response headers, which are terminated by a blank line.
    asio::read_until(socket, response, "\r\n\r\n");

    // Process the response headers.
    std::string header;
    while (std::getline(response_stream, header) && header != "\r") { }

    // Write whatever content we already have to output.
    if (response.size() > 0) {
        outFile << &response;
    }
    // Read until EOF, writing data to output as we go.
    while (asio::read(socket, response, asio::transfer_at_least(1), error)) {
        outFile << &response;
    }
}