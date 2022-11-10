#include "sslclient.h"

int main()
{
    char buf[BUFSIZE];
    SslClient client;
    client.connect("192.168.118.133", 9090);
    client.send("Test data to server", 500);
    client.recv(buf, BUFSIZE);
    DLOG(INFO) << buf;
}