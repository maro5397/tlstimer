#include "tcpclient.h"
#include <sys/stat.h>
#include <fstream>
#include <sys/time.h>
#include <unistd.h>

#define BUFLEN 1024+1

int delay = 0;
int view = 0;

double getConnTime(TcpClient* client, std::string ip, int port);
double getSendTime(TcpClient* client, std::string filename, int count);
int getFileSize(std::string filename);
void setSendBuffer(char sendbuf[], std::string filename, int start, int size);
void setDirAndLog();
void usage();

int main(int argc, char* argv[])
{
    setDirAndLog();
    if(argc < 7) {
        DLOG(INFO) << "usage incorrect";
        usage();
        exit(-1);
    }

    std::string ip(argv[1]);
    int port = atoi(argv[2]);
    std::string filename(argv[3]);
    int count = atoi(argv[4]);
    view = atoi(argv[5]);
    delay = atoi(argv[6]);
    
    DLOG(INFO) << "usage correct parse complete";
    DLOG(INFO) << "ip: " << ip;
    DLOG(INFO) << "port: " << port;
    DLOG(INFO) << "filename: " << filename;
    DLOG(INFO) << "view data: " << view;
    DLOG(INFO) << "delay(microsecond): " << delay;

    TcpClient* client = new TcpClient();
    getConnTime(client, ip, port);
    getSendTime(client, filename, count);
    client->disconnect();
    return 0;
}

double getSendTime(TcpClient* client, std::string filename, int count)
{
    struct timeval start, finish;
	double spent = 0.0;
    char sendbuf[BUFLEN];
    char recvbuf[BUFLEN];
    int size = getFileSize(filename);

    for(int i = 0; i < count; i++)
    {
        DLOG(INFO) << "Case #" << i + 1;
        for(int j = 0; j < size; j += BUFLEN-1)
        {
            setSendBuffer(sendbuf, filename, j, size);
            if(view) {
                DLOG(INFO) << "\n===============send data from client===============\n" 
                           << sendbuf 
                           << "\n===============send data from client===============\n";
            }
            gettimeofday(&start, NULL);
            if(client->send(sendbuf, strlen(sendbuf)) == -1) {
                exit(-1);
            }
            if(client->recv(recvbuf, BUFLEN) == -1) {
                exit(-1);
            }
            finish = *((struct timeval*)recvbuf);
            spent += (finish.tv_sec - start.tv_sec) + ((finish.tv_usec - start.tv_usec) / 1000000.0);
            DLOG(INFO) << "client start clock: " << start.tv_sec * 1000000 + start.tv_usec << "microseconds";
            DLOG(INFO) << "server finish clock: " << finish.tv_sec * 1000000 + finish.tv_usec << "microseconds";
            DLOG(INFO) << "spent clock: " << spent << "s";
            usleep(delay);
        }
    }
    DLOG(INFO) << "average of data transmission time(s): " << spent / count << "s";
    return spent / count;
}

void setSendBuffer(char sendbuf[], std::string filename, int start, int size)
{
    memset(sendbuf, 0, BUFLEN);
    std::ifstream fin("files/" + filename);
    if(fin.is_open()) {
        DLOG(INFO) << "Set start point at " << start;
        fin.seekg(start, std::ios::beg);
        if(start + BUFLEN-1 > size) {
            DLOG(INFO) << "send data size(byte): " << size - start;
            fin.read(sendbuf, size - start);
        }
        else {
            DLOG(INFO) << "send data size(byte): " << BUFLEN-1;
            fin.read(sendbuf, BUFLEN-1);
        }
    }
    else {
        DLOG(INFO) << "There is no files";
        exit(-1);
    }
    fin.close();
}

int getFileSize(std::string filename)
{
    std::ifstream fin("files/" + filename);
    int size = 0;
    
    if(fin.is_open()) {
        fin.seekg(0, std::ios::end);
        size = fin.tellg();
    }
    else {
        DLOG(INFO) << "There is no files";
        exit(-1);
    }
    fin.close();
    DLOG(INFO) << "entire data size(byte): " << size << "byte(s)";
    return size;
}

double getConnTime(TcpClient* client, std::string ip, int port)
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
	DLOG(INFO) << "syntax : main <server-ip> <server-port> <filename> <count> <view data> <delay>";
	DLOG(INFO) << "sample : main 192.168.1.2 8080 testdata.txt 30 0 0";
}