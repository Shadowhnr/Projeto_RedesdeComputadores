#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>
#define N 5
void error(char *msg)
{
    perror(msg);
    exit(1);
}
void communicate(int *sock)
{
   	int n;
   	char buffer[1024];
   	 printf("%d\n",(*sock));
   	memset(buffer,'\0',1024);
   	n = read((*sock),buffer,1024);
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
	
int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen,i;
    pthread_t threadsOfAcceptance[N];
     unsigned cli;
     struct sockaddr_in serv_addr, cli_addr;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     cli=(unsigned)clilen;
     while (1) {
         newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &cli);
         if (newsockfd < 0) 
             error("ERROR on accept");
         i=0;
         printf("%d\n",newsockfd);
         
         if((pthread_create(&threadsOfAcceptance[i],0,(void*)communicate,&newsockfd))!=0)
         {
             error("Error creating thread");
         }
        
         i++;
     }
     return 0;
}


