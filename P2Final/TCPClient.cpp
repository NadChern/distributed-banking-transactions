/**
 * @file TCPClient.cpp definition for TCPClient
 * @author Kevin Lundeen
 * @see Seattle University, CPSC 5042, Spring 2024, ICE 4 professor's solution
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include "TCPClient.h"

using namespace std;

TCPClient::TCPClient(const string &server_host, const u_short server_port) {
   s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
        throw runtime_error(strerror(errno));

    hostent *answer;
    answer = gethostbyname(server_host.c_str());
    if (answer == nullptr)
        throw runtime_error(strerror(h_errno));

    sockaddr_in to = {};
    to.sin_family = AF_INET;
    to.sin_addr.s_addr = *(in_addr_t *)answer->h_addr;
    to.sin_port = htons(server_port);
    if (connect(s, (struct sockaddr *) &to, sizeof(to)) < 0)
        throw runtime_error(strerror(errno));
}

TCPClient::~TCPClient() {
    if (s != -1) {
        close(s);
        s = -1;
    }
}

TCPClient::TCPClient(TCPClient &&other) noexcept {
    this->s = other.s;
    other.s = -1;
}

void TCPClient::send_request(const string &request) const {
    const char *message = request.c_str();
    ssize_t length = strlen(message);
    ssize_t sent = send(s, message, length, 0);
    if (sent < 0)
        throw runtime_error(strerror(errno));
}

string TCPClient::get_response() const {
    char buffer[4096]; // Declare a buffer to hold incoming data (4096 bytes)
    ssize_t received = recv(s, buffer, sizeof(buffer), 0); // Receive data from the socket
    if (received < 0)
        throw runtime_error(strerror(errno));
    buffer[received] = '\0'; // Null-terminate the received data to make it a valid C-string
    return string(buffer); // Convert the C-string to a C++ string and return it
}
