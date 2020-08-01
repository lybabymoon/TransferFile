#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <errno.h>
#include <assert.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <functional>
#include <thread>
using namespace std;
const int BUF_SIZE = 4096;
const int BACKLOG = 10;



const char* m_IpAddress = "192.168.127.131";
const int m_Port = 9000;

void* pthread_func(void *arg)
{
    int client_fd = 0;

    char buf[BUF_SIZE];
    char filename[BUF_SIZE];
    int count = 0;

    bzero(buf,BUF_SIZE);
    client_fd = *(int *)arg;
    cout << "recv from client,client_fd = " << client_fd << endl;

    count = recv(client_fd,buf,BUF_SIZE,0);
    if(count < 0)
    {
        perror("recv");
    }

    strncpy(filename,buf,strlen(buf) > BUF_SIZE ? BUF_SIZE : strlen(buf));
    //filename = {"123.txt"};
    cout << "file name is:" << filename << endl;

    FILE *fd = fopen(filename,"wb+");
    if(NULL == fd)
    {
        cout << "open failed!" << endl;
        exit(1);
    }

    bzero(buf,BUF_SIZE);

    int ret = 0;
    while(ret = recv(client_fd,buf,BUF_SIZE,0))
    {
        if(ret < 0)
        {
            cout << "recv failed!" << endl;
            exit(1);
        }

        int writenlen = fwrite(buf,sizeof(char),ret,fd);
        if(writenlen < ret)
        {
            cout << "write failed!" << endl;
        }
        bzero(buf,BUF_SIZE);
    }

    cout << "Receieved file: " << filename << " finished" << endl;
    fclose(fd);
    close(client_fd);
}

void recv_mul_file(int sockfd)
{
    pthread_t tid;
    struct sockaddr_in client_address;
    int client_fd,sin_size = sizeof(struct sockaddr);
    while(1)
    {
        if((client_fd = accept(sockfd,(struct sockaddr*)&client_address,(socklen_t *)&sin_size)) == -1)
        {
            cout << "accept socket error " << strerror(errno) << " " << errno << endl;
            continue;
        }
        int ret = pthread_create(&tid,NULL,pthread_func,&client_fd);
          //pthread_create(&tid,NULL,&Server::test,this);
          //std::thread(std::bind(&Server::pthread_func,this,client_fd));
          //std::thread(std::bind(&ClassX::functionA,&CA,_1,_2));
    }
}

void createSocket()
{
    //char buffer[BUF_SIZE] = {0};
    /*FILE *fp = fopen("output.txt","wb");
    if(fp == nullptr)
    {
        cout << "Can not open file" << endl;
        exit(0);
    }*/

    struct sockaddr_in client_address;
    bzero(&client_address,sizeof(client_address));
    client_address.sin_family = AF_INET;
    inet_pton(AF_INET,m_IpAddress,&client_address.sin_addr);
    //client_address.sin_addr.s_addr = INADDR_ANY;
    client_address.sin_port = htons(m_Port);

    int listenfd = socket(PF_INET,SOCK_STREAM,0);
    if(listenfd < 0)
    {
        cout << "create socket failed" << endl;
        return;
    }
    int ret = 0;


    ret = bind(listenfd,(struct sockaddr*)&client_address,sizeof(client_address));
    if(ret < 0)
    {
        cout << "binding failed" << endl;
        exit(0);
    }
    ret = listen(listenfd,BACKLOG);
    if(ret == -1)
    {
        cout << "listening failed" << endl;
        exit(0);
    }

    //int connfd = 0;
    cout << "waiting for cilent request" << endl;

    /*while(1)
    {

        memset(buffer,'\0',BUF_SIZE);
        socklen_t client_addrlength = sizeof(client_address);
        if((connfd = accept(listenfd,(struct sockaddr*)&client_address,&client_addrlength)) < 0)
        {
            cout << "accept socket error" << strerror(errno) << " " << errno << endl;
            continue;
        }


        int nCount = 0;
        while((nCount =  recv(connfd,buffer,BUF_SIZE,0)) > 0)
        {
            fwrite(buffer,nCount,1,fp);
        }


        nCount = recv(connfd,buffer,BUF_SIZE-1,0);
        if(nCount < 0)
        {
            cout << strerror(errno) <<endl;
            cout << "Recv failed" << endl;
            continue;
        }
        //cout << nCount << endl;
        cout << buffer << endl;
       // fclose(fp);
        close(connfd);buffer
    }
    */
    recv_mul_file(listenfd);
    close(listenfd);

    return ;
}


int main()
{
    createSocket();
    return 0;
}
