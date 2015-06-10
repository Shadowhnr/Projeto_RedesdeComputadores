#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    char buffer[1024];
    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(1);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("Error opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"Error, no such host\n");
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("Error connecting");
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
	return 0;
}
