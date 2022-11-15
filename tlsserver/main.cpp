#include "sslserver.h"
#include <sys/stat.h>
#include <fstream>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>

#define BUFLEN 1024+1

class TimerSsl : public SslServer
{
    std::string numoffiles_;

public:
    TimerSsl(double version);
    ~TimerSsl() {}

protected:
    void handleClnt(SslClientSocket* clntsock) override;
    void saveFile(char* recvbuf);
    bool setFileName();
};

TimerSsl::TimerSsl(double version) : SslServer(version) {
}

void TimerSsl::handleClnt(SslClientSocket* clntsock) {
    setFileName();
    char recvbuf[BUFLEN];
    int len = 0;
    int count = 0;
    struct timeval finish;
    long totallen = 0;
    while((len = clntsock->recv(recvbuf, BUFLEN)) != -1) {
        if(len == 0) {
            DLOG(INFO) << "total length of recv data(byte): " << totallen << "byte(s)";
            DLOG(INFO) << "clntsock is shutdown";
            return;
        }
        totallen += len;
        gettimeofday(&finish, NULL);
        char* sendbuf = (char*)&finish;
        clntsock->send(sendbuf, sizeof(struct timeval));
        DLOG(INFO) << "recv data number: " << count++;
        DLOG(INFO) << "\n===============recv data from client===============\n" 
                   << recvbuf 
                   << "\n===============recv data from client===============\n";
        DLOG(INFO) << "finish clock: " << finish.tv_sec * 1000000 + finish.tv_usec << "microseconds";
        DLOG(INFO) << "recv length(byte): " << len << "byte(s)";
        saveFile(recvbuf);
    }
    DLOG(INFO) << "abnormal disconnection from server";
    return;
}

bool TimerSsl::setFileName()
{
    DIR *dp;
    int numoffiles = -1;
    struct dirent *ep;
    dp = opendir("./files");
    if (dp != NULL) {
        while (ep = readdir (dp))
        {
            numoffiles++;
        }
        (void) closedir (dp);
    }
    else {
        DLOG(INFO) << "Couldn't open the directory";
        return false;
    }
    numoffiles_ = std::to_string(numoffiles);
    DLOG(INFO) << "There's " + numoffiles_ + " files in the current directory";
    return true;
}

void TimerSsl::saveFile(char* recvbuf)
{
    std::ofstream fout("files/recvfile" + numoffiles_ + ".txt", std::ios::app);
    if(fout.is_open()) {
        fout.write(recvbuf, strlen(recvbuf));
    }
    fout.close();
    return;
}

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

void usage()
{
	DLOG(INFO) << "syntax : main <server-port> <ciphersuite> <symmetric type> <certificate type>";
	DLOG(INFO) << "sample : main 8080 aes256 4096";
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