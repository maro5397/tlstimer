#include "sslserver.h"

class EchoSsl : public SslServer
{
public:
    EchoSsl();
    ~EchoSsl() {}

protected:
    void handleClnt(SslClientSocket* clntsock) override;
};

EchoSsl::EchoSsl() {
}

void EchoSsl::handleClnt(SslClientSocket* clntsock) {
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
    EchoSsl server;
    server.start(9090, "../certfiles/aes128/2048/cert.pem", "../certfiles/aes128/2048/key.pem");
    sleep(60);
    server.stop();
    return 0;
}