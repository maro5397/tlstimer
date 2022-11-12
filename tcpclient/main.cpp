#include "tcpclient.h"
#include <sys/stat.h>
#include <ctime>
#include <fstream>

float getConnTime(TcpClient* client, std::string ip, int port);
float getSendTime(TcpClient* client, std::string filename);
void setDirAndLog();
void usage();

int main(int argc, char* argv[])
{
    setDirAndLog();
    if(argc < 5) {
        DLOG(INFO) << "usage incorrect";
        usage();
        exit(-1);
    }

    std::string ip(argv[1]);
    int port = atoi(argv[2]);
    std::string filename(argv[3]);
    int count = atoi(argv[4]);
    float spent = 0.0;
    
    DLOG(INFO) << "usage correct parse complete";
    DLOG(INFO) << "ip: " << ip;
    DLOG(INFO) << "port: " << port;
    DLOG(INFO) << "filename: " << filename;

    TcpClient* client = new TcpClient();
    getConnTime(client, ip, port);
    for(int i = 0; i < count; i++) {
        DLOG(INFO) << "Case #" << i;
        spent += getSendTime(client, filename);
        usleep(500000);
    }
    DLOG(INFO) << "average of send package: " << spent / count << "s";
    client->disconnect();
}

float getSendTime(TcpClient* client, std::string filename)
{
    clock_t start, finish;
    float spent;
    char sendbuf[BUFSIZE];
    char recvbuf[BUFSIZE];
    std::ifstream fin("files/" + filename);
    int size = 0;
    
    if(fin.is_open()) {
        fin.seekg(0, std::ios::end);
        size = fin.tellg();
        fin.seekg(0, std::ios::beg);
        fin.read(sendbuf, size);
    }
    fin.close();
    
    DLOG(INFO) << "data size(byte): " << size << "byte(s)";

    start = clock();
    if(client->send(sendbuf, strlen(sendbuf)) == -1) {
        exit(-1);
    }
    if(client->recv(recvbuf, BUFSIZE) == -1) {
        exit(-1);
    }
    finish = *((long*)recvbuf);

    spent = (float)(finish - start) / CLOCKS_PER_SEC;
    DLOG(INFO) << "server finish clock: " << finish;
    DLOG(INFO) << "data transmission time(s): " << spent << "s";
    return spent;
}

float getConnTime(TcpClient* client, std::string ip, int port)
{
    clock_t start, finish;
    float spent;

    start = clock();
    if(client->connect(ip, port) == -1) {
        exit(-1);
    }
    finish = clock();

    spent = (float)(finish - start) / CLOCKS_PER_SEC;
    DLOG(INFO) << "create connection time(s): " << spent << "s";
    return spent;
}

void setDirAndLog()
{
    if (mkdir("logs", 0777) == -1)
        DLOG(INFO) << "logs directory create Error : " << strerror(errno);
    else
        DLOG(INFO) << "logs directory create Success";

    if (mkdir("files", 0777) == -1)
        DLOG(INFO) << "files directory create Error : " << strerror(errno);
    else
        DLOG(INFO) << "files directory create Success";
    
    FLAGS_alsologtostderr = 1;
    google::SetLogDestination(google::INFO, "logs/");
    google::InitGoogleLogging("logs/");
}

void usage()
{
	DLOG(INFO) << "syntax : main <server-ip> <server-port> <filename> <count>";
	DLOG(INFO) << "sample : main 192.168.1.2 8080 testdata.txt 30";
}