#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <string>
#include <stdexcept>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <thread>
#include <mutex>
#include <vector>

class TCPConnection {
public:
    TCPConnection(const std::string& ip, int port);
    ~TCPConnection();
    bool connectToServer();
    bool sendData(const uint8_t* data, size_t size);
    void sendInitialMessages();
    void startReceiving();
    void stopReceiving();

private:
    int client_socket;
    std::string server_ip;
    int server_port;
    struct sockaddr_in server_addr;
    std::thread receive_thread;
    std::mutex socket_mutex;
    bool running;

    bool setNonBlocking();
    bool waitForConnection();
    void receiveMessages();
};

#endif // TCP_CONNECTION_H
