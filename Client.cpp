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
#include <netdb.h>
#include <pthread.h>
#include <sys/ioctl.h>
using namespace std;

const int BUF_SIZE = 1024;

class Client
{
    const char* m_IpAdress;
    const int m_Port;
  public:
    Client(string _ip,int _port):
        m_IpAdress(_ip.c_str()),
        m_Port(_port)
    {}

    void createClient()
    {
        struct sockaddr_in server_address;
        //struct hostent *host;

        /*if((host = gethostbyname(m_IpAdress)) == nullptr)
        {
            perror("gethostbyname");
            exit(1);
        }*/

        bzero(&server_address,sizeof(server_address));
        server_address.sin_family = AF_INET;
        inet_pton(AF_INET,m_IpAdress,&server_address.sin_addr);
       // server_address.sin_addr.s_addr = *((struct in_addr*)host->h_addr);
        server_address.sin_port = htons(m_Port);

        int sockfd = socket(PF_INET,SOCK_STREAM,0);
        if(sockfd < 0)
        {
            cout << "create socket failed" << endl;
            exit(0);
        }

        if(connect(sockfd,(struct sockaddr*)&server_address,sizeof(server_address)) < 0)
        {
            cout << "conncect failed" << endl;
            exit(0);
        }
        /*
        else
        {
            cout << "send data" << endl;
            fgets(buf,BUF_SIZE,stdin);
            if(send(sockfd,buf,strlen(buf),0) < 0)
            {
                cout << "send failed" << endl;
            }


        }
        */

        //send file
        char filename[BUF_SIZE];
        bzero(filename,BUF_SIZE);
        char buf[BUF_SIZE];
        bzero(buf,BUF_SIZE);
        int ret = 0;
        cout << "Please input filename: ";
        cin >> filename;
        cout << endl;

        strncpy(buf,filename,strlen(filename) > BUF_SIZE ? BUF_SIZE : strlen(filename));
        ret = send(sockfd,buf,BUF_SIZE,0);
        if(ret < 0)
        {
            perror("send file name");
            exit(1);
        }

        //read file
        FILE *fd = fopen(filename,"rb");
        if(fd == nullptr)
        {
            cout << "File: " << filename << " not found!!" << endl;
        }
        else
        {
            bzero(buf,BUF_SIZE);
            size_t file_block_length = 0;
            while((file_block_length = fread(buf,sizeof(char),BUF_SIZE,fd)) > 0)
            {
                //cout << "file_block_length" << file_block_length << endl;

                if(send(sockfd,buf,file_block_length,0) < 0)
                {
                    perror("Send");
                    exit(1);
                }

                bzero(buf,BUF_SIZE);
            }

            fclose(fd);
            cout << "Tranfer file finished!" << endl;
        }

        close(sockfd);
    }
};

int main()
{
    Client client("192.168.127.131",9000);
    client.createClient();
    return 0;
}
