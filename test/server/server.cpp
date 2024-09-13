#include "Server.h"
#include <iostream>
#include "Buffer.h"
#include "Connection.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Logger.h"

void handleConnection(Connection* conn);

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "input format: .out port *.log" << std::endl;
        return -1;
    }
    uint16_t port = std::stoi(argv[1]);
    std::string log_name = argv[2];
    std::string log_path = "../log/" + log_name;

    Logger::instance(log_path.c_str()).start();
    LOG_INFO("======================= log start! ========================");

    LOG_INFO("Create a server");
    Server* server = new Server(port, EPOLLIN);
    LOG_INFO("to onConnection");
    server->onConnection(&handleConnection);
    server->Start();
    Logger::instance().stop();
    LOG_INFO("===========================================================");
    delete server;
    return 0;
}

void handleConnection(Connection* conn) {
    LOG_INFO("handleConnection Function --- http Service");

    LOG_INFO("==== request message! ====");
    Buffer* readBuffer = conn->getReadBuffer();
    LOG_INFO("%s", readBuffer->toStr().c_str());
    HttpRequest request;
    if (!request.parse(*readBuffer)) {
        conn->Send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->Close();
        return;
    }

    LOG_INFO("==== response message! ====");
    HttpResponse response("../resources", request.path(), request.isKeepAlive(), 200);
    Buffer sendBuffer;
    response.makeResponse(sendBuffer);
    conn->Send(sendBuffer.toStr());
    if (!request.isKeepAlive()) {
        LOG_INFO("IS NOT KEEPLIVE!");
        conn->Close();
    }
}
