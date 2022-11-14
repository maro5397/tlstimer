#include "sslserver.h"
#include <sys/stat.h>
#include <fstream>
#include <sys/time.h>
#include <unistd.h>

class TimerSsl : public SslServer
{
public:
    TimerSsl(double version);
    ~TimerSsl() {}

protected:
    void handleClnt(SslClientSocket* clntsock) override;
    void saveFile(char* recvbuf);
};

TimerSsl::TimerSsl(double version) : SslServer(version) {
}

void TimerSsl::handleClnt(SslClientSocket* clntsock) {
    char recvbuf[BUFSIZE];
    int len = 0;
    int count = 0;
    struct timeval finish;
    bool issave = false;
    while((len = clntsock->recv(recvbuf, BUFSIZE)) != -1) {
        if(len == 0) {
            DLOG(INFO) << "clntsock is shutdown";
            return;
        }
        gettimeofday(&finish, NULL);
        char* sendbuf = (char*)&finish;
        clntsock->send(sendbuf, sizeof(struct timeval));
        DLOG(INFO) << "recv data number: " << count++;
        DLOG(INFO) << "\n===============recv data from client===============\n" 
                   << recvbuf 
                   << "\n===============recv data from client===============\n";
        DLOG(INFO) << "finish clock: " << finish.tv_sec * 1000000 + finish.tv_usec << "microseconds";
        if(!issave) {
            saveFile(recvbuf);
            issave = true;
        }
    }
    return;
}

void TimerSsl::saveFile(char* recvbuf)
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
	DLOG(INFO) << "syntax : main <server-port> <ciphersuite> <symmetric type> <certificate type>";
	DLOG(INFO) << "sample : main 8080 aes256 4096";
}

int main(int argc, char* argv[])
{
    if (mkdir("files", 0777) == -1)
        DLOG(INFO) << "files directory create Error : " << strerror(errno);
    else
        DLOG(INFO) << "files directory create Success";

    if(argc < 5) {
        DLOG(INFO) << "usage incorrect";
        usage();
        exit(-1);
    }

    int port = atoi(argv[1]);
    char* cipherlist = argv[2];
    std::string symtype(argv[3]);
    std::string certtype(argv[4]);

    DLOG(INFO) << "usage correct parse complete";
    DLOG(INFO) << "port: " << port;
    DLOG(INFO) << "cipherlist: " << cipherlist;
    DLOG(INFO) << "symmetric type: " << symtype;
    DLOG(INFO) << "certificate type: " << certtype;
    
    TimerSsl* server = new TimerSsl(1.2);
    server->start(port, 
                  "../certfiles/" + symtype + "/" + certtype + "/cert.pem", 
                  "../certfiles/" + symtype + "/" + certtype + "/key.pem",
                  cipherlist);
    sleep(6000);
    server->stop();
    return 0;
}
