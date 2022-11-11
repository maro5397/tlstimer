#include "tcpserver.h"
#include <sys/stat.h>
#include <ctime>
#include <fstream>

class TimerTcp : public TcpServer
{
public:
    TimerTcp();
    ~TimerTcp() {}

protected:
    void handleClnt(TcpClientSocket* clntsock) override;
    void saveFile(char* recvbuf);
};

TimerTcp::TimerTcp() {
}

void TimerTcp::handleClnt(TcpClientSocket* clntsock) {
    char recvbuf[BUFSIZE];
    int len = 0;
    int count = 0;
    clock_t finish = 0;
    while((len = clntsock->recv(recvbuf, BUFSIZE)) != -1) {
        if(len == 0) {
            DLOG(INFO) << "clntsock is shutdown";
            return;
        }
        finish = clock();
        char* sendbuf = (char*)&finish;
        clntsock->send(sendbuf, strlen(sendbuf));
        DLOG(INFO) << "recv data number: " << count++;
        DLOG(INFO) << "\n===============recv data from client===============\n" 
                   << recvbuf 
                   << "\n===============recv data from client===============\n";
        DLOG(INFO) << "finish clock: " << finish;
        //saveFile(recvbuf);
    }
    return;
}

void TimerTcp::saveFile(char* recvbuf)
{
    time_t rawtime;
    time(&rawtime);
    struct tm* timeinfo = localtime(&rawtime);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
    std::string datetime(buffer);

    std::ofstream fout("files/" + datetime + ".txt");
    if(fout.is_open()) {
        fout.write(recvbuf, strlen(recvbuf));
    }
    fout.close();
    return;
}

void usage()
{
	DLOG(INFO) << "syntax : main <server-port>";
	DLOG(INFO) << "sample : main 8080";
}

int main(int argc, char* argv[])
{
    if (mkdir("files", 0777) == -1)
        DLOG(INFO) << "files directory create Error : " << strerror(errno);
    else
        DLOG(INFO) << "files directory create Success";
    
    if(argc < 2) {
        DLOG(INFO) << "usage incorrect";
        usage();
        exit(-1);
    }

    int port = atoi(argv[1]);
    
    DLOG(INFO) << "usage correct parse complete";
    DLOG(INFO) << "port: " << port;

    TimerTcp* server = new TimerTcp();
    server->start(port);
    sleep(6000);
    server->stop();
    return 0;
}