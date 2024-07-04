#include "tcp_connection.h"
#include <iostream>

TCPConnection::TCPConnection(const std::string& ip, int port) 
    : server_ip(ip), server_port(port), running(false) {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        throw std::runtime_error("Socket creation failed: " + std::string(strerror(errno)));
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0) {
        close(client_socket);
        throw std::runtime_error("Invalid address/ Address not supported");
    }
}

TCPConnection::~TCPConnection() {
    stopReceiving();
    close(client_socket);
}

bool TCPConnection::setNonBlocking() {
    int flags = fcntl(client_socket, F_GETFL, 0);
    return (fcntl(client_socket, F_SETFL, flags | O_NONBLOCK) == 0);
}

bool TCPConnection::waitForConnection() {
    fd_set write_fds;
    FD_ZERO(&write_fds);
    FD_SET(client_socket, &write_fds);
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    if (select(client_socket + 1, nullptr, &write_fds, nullptr, &timeout) <= 0) {
        return false;
    }

    int so_error;
    socklen_t len = sizeof(so_error);
    getsockopt(client_socket, SOL_SOCKET, SO_ERROR, &so_error, &len);
    return (so_error == 0);
}

bool TCPConnection::connectToServer() {
    if (!setNonBlocking()) {
        return false;
    }

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        if (errno != EINPROGRESS) {
            return false;
        }
    }

    return waitForConnection();
}

bool TCPConnection::sendData(const uint8_t* data, size_t size) {
    std::lock_guard<std::mutex> lock(socket_mutex);
    return (send(client_socket, data, size, 0) >= 0);
}

void TCPConnection::sendInitialMessages() {
    uint8_t message1[13] = {0x08, 0x00, 0x00, 0x06, 0x01, 0x2F, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t message2[13] = {0x08, 0x00, 0x00, 0x06, 0x02, 0x2F, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00};

    for (int i = 0; i < 5; ++i) {
        sendData(message1, sizeof(message1));
        sendData(message2, sizeof(message2));
    }
}

void TCPConnection::startReceiving() {
    running = true;
    receive_thread = std::thread(&TCPConnection::receiveMessages, this);
}

void TCPConnection::stopReceiving() {
    running = false;
    if (receive_thread.joinable()) {
        receive_thread.join();
    }
}

void TCPConnection::receiveMessages() {
    while (running) {
        char buffer[1024];
        int bytes_received;
        {
            std::lock_guard<std::mutex> lock(socket_mutex);
            bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        }
        if (bytes_received > 0) {
            std::cout << "Received from server: ";
            for (int i = 0; i < bytes_received; ++i) {
                printf("%02x ", static_cast<unsigned char>(buffer[i]));
            }
            std::cout << std::endl;
        } else if (bytes_received == 0) {
            std::cout << "Server closed connection" << std::endl;
            stopReceiving();
            break;
        } else if (bytes_received < 0 && errno != EWOULDBLOCK) {
            std::cerr << "Socket error: " << strerror(errno) << std::endl;
            stopReceiving();
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
