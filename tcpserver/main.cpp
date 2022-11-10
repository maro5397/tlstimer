#include "tcpserver.h"

class EchoTcp : public TcpServer
{
public:
    EchoTcp();
    ~EchoTcp() {}

protected:
    void handleClnt(TcpClientSocket* clntsock) override;
};

EchoTcp::EchoTcp() {
}

void EchoTcp::handleClnt(TcpClientSocket* clntsock) {
    char buffer[BUFSIZE];
    char message[] = "HI, this is server";
    int len = 0;
    int count = 0;
    while((len = clntsock->recv(buffer, BUFSIZE)) != -1) {
        if(len == 0) {
            DLOG(INFO) << "clntsock is shutdown";
            return;
        }
        DLOG(INFO) << count++;
        DLOG(INFO) << "=====recv data from client=====";
        DLOG(INFO) << buffer;
        DLOG(INFO) << "=====recv data from client=====";
        clntsock->send(message, strlen(message) + 1);
    }
    return;
}

int main()
{
    EchoTcp server;
    server.start(9090);
    sleep(60);
    server.stop();
    return 0;
}