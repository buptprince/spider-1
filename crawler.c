#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "transfer.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>



#define DEFAULT_PAGE_BUF_SIZE 1024 * 1024

using namespace std;

char* ip;
char* port;

int crateSocket();
//crawler ip port url.txt
int main(int argc,char* argv[]){

    if(argc<4){
        printf("error,lack of para\n");
        exit(-1);
    }
    ip=argv[1];
    port=argv[2];
    urlfp=fopen(argv[3],"w");

    initBoolmFilter();
    int socket_client;
    int num = 0;
    int isIndex;
    int ContentLength = DEFAULT_PAGE_BUF_SIZE;
   
    char *curUrl;
    char start[]="news.sohu.com/";
   

    while(!isEmpty(waitUrlPool)){//保证开始时队列为空，可删去
        pop(waitUrlPool);
    }

    push(waitUrlPool,start);
    dokhash(start);
    addnewPage(start);
   
    char* msg=(char*)malloc(DEFAULT_PAGE_BUF_SIZE);

    while(!isEmpty(waitUrlPool)){
        
        curUrl=pop(waitUrlPool);
        char *request=makeRequest(curUrl);
        socket_client=crateSocket();

        while(send(socket_client,request,strlen(request),0)<0){
            printf("send error\n");
            printf( "%d ", errno);
            exit(-1);
        }

        int currecvlen=0;
        int ret=recv(socket_client,msg,DEFAULT_PAGE_BUF_SIZE,0);       
        msg[ret]='\0';
       
        int Content_Length=Validation(msg);

        if(Content_Length==-1){
            close(socket_client);
            continue;
        }

        currecvlen=ret;
        while(currecvlen < Content_Length) {
            currecvlen+=recv(socket_client,msg+currecvlen,DEFAULT_PAGE_BUF_SIZE-currecvlen,0);
        }
        close(socket_client);
        msg[currecvlen]='\0';
        htmlParse(msg,curUrl);
    }
    mergefile();
    printf("finish");
    return 0;
}


int crateSocket(){
    struct sockaddr_in serveraddr;
    int socket_client = socket(AF_INET,SOCK_STREAM,0);

    if(socket_client==-1){
        printf("socket error  %d===%d\n",socket_client,errno);
        exit(-1);
    }

    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(port));
    serveraddr.sin_addr.s_addr = inet_addr(ip);  


    if(connect(socket_client,(struct sockaddr*)&serveraddr,sizeof(serveraddr))){
        printf("send error  %d===%d\n",socket_client,errno);
        exit(-1);
    }
    return socket_client;
}