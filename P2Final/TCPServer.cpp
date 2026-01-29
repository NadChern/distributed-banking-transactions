/**
 * @file TCPServer.cpp definition for a base class TCP/IP server
 * @author Kevin Lundeen, Nadezhda Chernova
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include "TCPServer.h"


using namespace std;

TCPServer::TCPServer(u_short port) {
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0)
        throw runtime_error(
                string("Failed to create socket: ") + strerror(errno));

    if (port != 0) {
        sockaddr_in me = {};
        me.sin_family = AF_INET;
        me.sin_port = htons(port);
        me.sin_addr.s_addr = inet_addr("0.0.0.0");
        if (::bind(server, (sockaddr *) &me, sizeof(me)) < 0)
            throw runtime_error(
                    string("Failed to bind socket: ") + strerror(errno));
    }

    if (listen(server, 1) < 0)
        throw runtime_error(
                string("Failed to listen on socket: ") + strerror(errno));

    client = -1;
}

TCPServer::~TCPServer() {
    stopServer();
    closeClientSocket();
}

void TCPServer::stopServer() {
    if (server >= 0)
        close(server);
    server = -1;
}

void TCPServer::closeClientSocket() {
    if (client != -1) {
        close(client);
        client = -1;
    }
}

void TCPServer::serve() {
    sockaddr_in them = {};
    socklen_t them_len = sizeof(them);
    client = accept(server, (sockaddr *) &them, &them_len);
    if (client < 0)
        throw runtime_error(
                string("Failed to accept connection: ") + strerror(errno));

    string their_host = inet_ntoa(them.sin_addr);
    u_short their_port = ntohs(them.sin_port);
    start_client(their_host, their_port);
    try {
        while (true) {
            char buffer[1024];
            ssize_t received = recv(client, buffer, sizeof(buffer), 0);

            if (received == -1)
                throw runtime_error(
                        string("Failed to receive data: ") + strerror(errno));

            // Connection closed by the client
            if (received == 0) {
                cerr << "Connection closed by the client." << endl;
                closeClientSocket();   // close client socket
                break;
            }
            buffer[received] = '\0'; // null-terminate C-style string
            string request(buffer);
            if (!process(request)) {
                return;
            }
        }
    } catch (const std::exception &e) {
        cerr << e.what() << endl;
        closeClientSocket(); // ensures client socket is closed on exception
    }
}

void TCPServer::respond(const string &response) const {
    ssize_t sent = send(client, response.c_str(), response.length(), 0);
    if (sent < 0)
        throw runtime_error(
                string("Failed to send data: ") + strerror(errno));
}