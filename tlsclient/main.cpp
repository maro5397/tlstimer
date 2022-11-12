#include "sslclient.h"
#include <sys/stat.h>
#include <fstream>
#include <sys/time.h>
#include <unistd.h>

double getConnTime(SslClient* client, std::string ip, int port);
double getSendTime(SslClient* client, std::string filename, int count);
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
    
    DLOG(INFO) << "usage correct parse complete";
    DLOG(INFO) << "ip: " << ip;
    DLOG(INFO) << "port: " << port;
    DLOG(INFO) << "filename: " << filename;

    SslClient* client = new SslClient();
    getConnTime(client, ip, port);
    getSendTime(client, filename, count);
    client->disconnect();
    return 0;
}

double getSendTime(SslClient* client, std::string filename, int count)
{
    struct timeval start, finish;
	double spent = 0.0;
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

    for(int i = 0; i < count; i++)
    {
        DLOG(INFO) << "Case #" << i + 1;
        gettimeofday(&start, NULL);
        if(client->send(sendbuf, strlen(sendbuf)) == -1) {
            exit(-1);
        }
        if(client->recv(recvbuf, BUFSIZE) == -1) {
            exit(-1);
        }
        finish = *((struct timeval*)recvbuf);
        spent += (finish.tv_sec - start.tv_sec) + ((finish.tv_usec - start.tv_usec) / 1000000.0);
        DLOG(INFO) << "client start clock: " << start.tv_sec * 1000000 + start.tv_usec << "microseconds";
        DLOG(INFO) << "server finish clock: " << finish.tv_sec * 1000000 + finish.tv_usec << "microseconds";
        DLOG(INFO) << "spent clock: " << (finish.tv_sec - start.tv_sec) + ((finish.tv_usec - start.tv_usec) / 1000000.0) << "s";
        usleep(500000);
    }
    DLOG(INFO) << "average of data transmission time(s): " << spent / count << "s";
    return spent / count;
}

double getConnTime(SslClient* client, std::string ip, int port)
{
    struct timeval start, finish;
	double spent;

    gettimeofday(&start, NULL);
    if(client->connect(ip, port) == -1) {
        exit(-1);
    }
    gettimeofday(&finish, NULL);

    spent = (finish.tv_sec - start.tv_sec) + ((finish.tv_usec - start.tv_usec) / 1000000.0);

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