#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#define N 128
typedef struct
{
	char[50] name;
	char[50] IP;
}contact;
void error(char *msg)
{
	perror(msg);
	exit(1);
}
void error_thread()
{
    error("Error creating thread!");
}
void communicate(int *sock)
{
	int n;
   	char buffer[1024];
    printf("%d\n",(*sock));
   	memset(buffer,'\0',1024);
   	n = read((*sock),buffer,1024);
    if(n<0)
    {
        error("Error reading from socket");
    }
    while(strcmp(buffer,"exit\n")!=0)
    {
        printf("%s\n",buffer);
        memset(buffer,'\0',1024);
        fgets(buffer,1024,stdin);
        n = write((*sock),buffer,1024);
        if (n < 0)
        {
            error("Error writing to socket");
        }
        if(strcmp(buffer,"exit\n")==0)
        {
            break;
        }
        memset(buffer,'\0',1024);
        n=read((*sock),buffer,1024);
        if(n<0)
        {
            error("Error reading from socket");
        }
    }
	close((*sock));
    pthread_exit(0);
}
void client()
{
	int sockfd,n;
	char sv_name[50];
	struct sockaddr_in serv_addr;
    	struct hostent *server;
   	char buffer[1024];
    	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
		error("Error opening socket");
	}
	printf("Type in the server's IP address\n");
	scanf("%s",sv_name);
	server=gethostbyname(sv_name);
	if(server==NULL)
	{
		fprintf(stderr,"Error, no such host!\n");
		exit(1);
	}
	memset((char *) &serv_addr,'\0',sizeof(serv_addr));
    	serv_addr.sin_family = AF_INET;
   	memcpy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    	serv_addr.sin_port = htons(7000);
    	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
	{
		error("Error connecting");
	}
	memset(buffer,'\0',1024);
	fgets(buffer,1024,stdin);
	while(strcmp(buffer,"exit\n")!=0)
	{
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0)
        {
            error("Error writing to socket");
        }
        memset(buffer,'\0',1024);
        n = read(sockfd,buffer,1024);
        if (n < 0)
        {
            error("Error reading from socket");
        }
        if(strcmp(buffer,"exit\n")==0)
        {
            break;
        }
        printf("%s\n",buffer);
        memset(buffer,'\0',1024);
        fgets(buffer,1024,stdin);
	}
    close(sockfd);
}
void menu()
{
    int choice=0;
    printf("What operation do you want to perform?\n1-Add Contact\n2-List Contacts\n3-Delete Contact\n4-Send a message\n5-Group message\n6-View incoming messages\n7-Quit\n");
    while(choice!=7)
    {
        scanf("%d",&choice);
        switch (choice)
        {
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                client();
                break;
            case 5:
                break;
            case 6:
                break;
            default:
                break;
        }
    }
}
void server()
{
    int sockfd, newsockfd, portno, clilen,i;
    pthread_t threadsOfAcceptance[N];
    unsigned cli;
    struct sockaddr_in serv_addr, cli_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("Error opening socket");
    }
    memset((char *) &serv_addr,'\0',sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(7000);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        error("Error on binding");
    }
    listen(sockfd,128);
    clilen = sizeof(cli_addr);
    cli=(unsigned)clilen;
    i=0;
    while (1)
    {
        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &cli);
        if (newsockfd < 0)
        {
            error("Error on accept");
        }
        if((pthread_create(&threadsOfAcceptance[i],0,(void*)communicate,&newsockfd))!=0)
        {
            error("Error creating thread");
        }
        i++;
    }
}
int main()
{
    int choice;
    pthread_t thread_menu,thread_server;
    if(pthread_create(&thread_server,0,&server,0)!=0)
    {
        error_thread;
    }
    if(pthread_create(&thread_menu,0,&menu,0)!=0)
    {
        error_thread;
    }
    return 0;
}
