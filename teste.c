#include <sys/types.h>   // Definicao de tipos
#include <sys/socket.h>  // Biblioteca de estrutara de sockets
#include <netinet/in.h>  // Define os padroes de protocolo IP
#include <netdb.h>
#include <arpa/inet.h>   // Converte enderecos hosts
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Define constantes POSIX
#include <errno.h>  // Utilizado para capturar mensagens de erro
#include <pthread.h>
#define MAX_USERS 10

//lista de contatos
 struct {
      int id;   // variavel de conexao, connected[id]
      int validade;        
      char name[150];
      int status;
      char server_name[512];
   } contatos[MAX_USERS];
//usado para associar contatos com idconnect, idconnect representa 1 conexao
 struct{
      int status;
      int contatos;
} sIdConnect[MAX_USERS];

int i;
	char name[150];
//cliente 
  	struct hostent *cl_Host[MAX_USERS];
	struct sockaddr_in cl_Server_addr[MAX_USERS];
//servidor
	int sv_Sock,true = 1;
	struct sockaddr_in sv_Server_addr, sv_Client_addr;
	int sin_size;
//ambos
	char receveDate[MAX_USERS][1024];
	int connected[MAX_USERS],idConnect=0;
	char send_data[1024];

//envia o nome da maquina para iniciar conexao
int protocolClient(int i){
 	send(connected[i],name,strlen(name), 0);
	return 0;
}

//recebe o nome da maquina que solicitou conexao e o adiciona em contatos
int protocolServer(int i){
	int bytes_recv;
	int k;
	for(k=0;k<MAX_USERS;k++){
		if(contatos[k].validade==0){
				bytes_recv=recv(connected[i],contatos[k].name,1024,0);
      				contatos[k].validade=1;
     				contatos[k].status=1;
				contatos[k].id=i;

        			sIdConnect[i].status=1;
        			sIdConnect[i].contatos=k;

				printf("------------------\n");
				printf("contato adicionado\n");
				printf("------------------\n");
				printf("nome %s\n",contatos[k].name);
				printf("nome server %s\n",contatos[k].server_name);
				printf("validade %d\n",contatos[k].validade);
				printf("status %d\n",contatos[k].status);
				printf("id %d\n",contatos[k].id);
				printf("k %d\n",k);
				printf("------------------\n");				
				return 0;
			}

	}
}

//verifica se id connect esta disponivel
void verIdConnect(){
	if(idConnect>=MAX_USERS)idConnect=0;
	if(sIdConnect[idConnect].status==1){
			idConnect++;
			verIdConnect();
		}
	
}
//conecta ao contato i
void client(int i){
   idConnect++; //idconnect, qual connected[] esta disponivel
   verIdConnect(); // verifica se idConnect esta em uso

   if ((connected[idConnect] = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
     perror("Erro no Socket");
     exit(1);
   }

   cl_Host[idConnect] = gethostbyname(contatos[i].server_name);
   cl_Server_addr[idConnect].sin_family = AF_INET;
   cl_Server_addr[idConnect].sin_port = htons(8000);
   cl_Server_addr[idConnect].sin_addr = *((struct in_addr *)cl_Host[idConnect]->h_addr);
   bzero(&(cl_Server_addr[idConnect].sin_zero),8);
 
   if (connect(connected[idConnect], (struct sockaddr *)&cl_Server_addr[idConnect], sizeof(struct sockaddr)) == -1)
   {
      perror("Erro de conexao verifique o ip do contato");
      return;
   }  contatos[i].status=1;
      contatos[i].id=idConnect;
      sIdConnect[idConnect].status=1;
      sIdConnect[idConnect].contatos=i;

   if(protocolClient(idConnect)==1){
	close(connected[idConnect]);	
	}
}
//recebe conexao e o aceita em connected[idConnect]

void server(){
   idConnect++;
   verIdConnect(); // verifica se idConnect esta em uso
   sin_size = sizeof(struct sockaddr_in);
   connected[idConnect] = accept(sv_Sock, (struct sockaddr *)&sv_Client_addr, &sin_size);
   printf("conexao aceita \n");
   if(protocolServer(idConnect)==1){
      close(connected[idConnect]);	
   }
}
void serverInit(){
   idConnect++;
   verIdConnect(); // verifica se idConnect esta em uso
   if ((sv_Sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
     perror("Erro no Socket");
     exit(1);
	}

   if (setsockopt(sv_Sock, SOL_SOCKET, SO_REUSEADDR, &true,sizeof(int)) == -1)
   {
      perror("Erro Setsockopt");
      exit(1);
   }


   // Configura o endereco de destino

   sv_Server_addr.sin_family = AF_INET;
   sv_Server_addr.sin_port = htons(8000);
   sv_Server_addr.sin_addr.s_addr = INADDR_ANY;
   bzero(&(sv_Server_addr.sin_zero),8);

 if (bind(sv_Sock, (struct sockaddr *)&sv_Server_addr, sizeof(struct sockaddr)) == -1)
   {
      perror("Nao foi possivel realizar o bind");
      exit(1);
   }
if (listen(sv_Sock, 10) == -1)
   {
      perror("Erro de Listen");
      exit(1);
   }
        sin_size = sizeof(struct sockaddr_in);
	connected[idConnect] = accept(sv_Sock, (struct sockaddr *)&sv_Client_addr, &sin_size);
	printf("conexao aceita \n");
        if(protocolServer(idConnect)==1){
	     close(connected[idConnect]);	
	}
	
}

void adicionar(){
	for(i=0;i<MAX_USERS;i++){
		if(contatos[i].validade==0){
				printf("digite o nome\n");
				scanf("%s",contatos[i].name);
				printf("digite o IP\n");
				scanf("%s",contatos[i].server_name);			
      				contatos[i].validade=1;
     				contatos[i].status=0;
				client(i);//estabelece conexao
				printf("------------------\n");
				printf("contato adicionado\n");
				printf("------------------\n");
				printf("nome %s\n",contatos[i].name);
				printf("nome server %s\n",contatos[i].server_name);
				printf("validade %d\n",contatos[i].validade);
				printf("status %d\n",contatos[i].status);
				printf("id %d\n",contatos[i].id);
				printf("i %d\n",i);
				printf("------------------\n");

				return;
			}

	}
}

void enviar(){
	char name[150];
	while(1){
		printf("digite o nome do contato ou <sair>\n");
		scanf("%s",name);
		if(strcmp(name,"sair")==0){return;}
		for(i=0;i<MAX_USERS;i++){
			if(strcmp(name,contatos[i].name)==0 && contatos[i].validade==1){
					if(contatos[i].status==0){
						//estabelece conexao
						//client(idConnect);
						}	
					//envia mensagen
					printf("digite a mensagem \n");
                   // fgets(send_data, 1024, stdin);
				scanf("%s",send_data);
					send(connected[contatos[i].id],send_data,strlen(send_data), 0);
					}			
			}		
		}
	}



//thread de conexoes, aceita conexoes na porta 8000
void *connectListen(){
	serverInit();
	while(1)	
	server();
}

//fecha conexao

void closeConnection(int id){
        int user;
	user=sIdConnect[id].contatos;
	sIdConnect[id].status=0;
	contatos[user].status=0;
	contatos[user].validade=0;	
	close(connected[id]);
	printf("usuario %s foi desconectado \n",contatos[user].name);
}
//thread de receve, recebe msgs do connected[i]
int x=0;
void *receve()
{

sleep((x+1)/100);
int id=x;
x++;

int bytes_recv;
while(1){		

	if(sIdConnect[id].status==1){
		sleep(1);
		bytes_recv=recv(connected[id],receveDate[id],1024,0);
		if(bytes_recv<1){
		closeConnection(id);
		}else{
                printf("tamanho da msg %d\n",bytes_recv);			
                printf("mensagens recebida: %s\n",receveDate[id]);
		}
	}
    }
}
//mostra contatos online
void listarContatos(){
printf("\ncontatos online\n");
int i;
for(i=0;i<MAX_USERS;i++){
   if(contatos[i].status==1){
		printf("%s\n",contatos[i].name);
	}
}
printf("\n");
}
//remove contatos
void removeContatos(){
	printf("digite o nome do contato a ser removido\n");
	int i;
	char name[150];
	scanf("%s",name);
        for(i=0;i<MAX_USERS;i++){
        if(contatos[i].status==1 && strcmp(name,contatos[i].name)==0){
	     closeConnection(contatos[i].id);
	     return;
	}
        
   }
printf("contato nao encontrado\n");

}
//graficos
void printMenu(){
        printf("Menu principal\n");
	printf("1 - adicionar contatos\n");
	printf("2 - enviar mensagens\n");
	printf("3 - exibir contatos online\n");
	printf("4 - remover contato\n");
	printf("5 - sair\n");
}
void closeAllConnection(){
	int i;	
	for(i=0;i<MAX_USERS;i++){
		closeConnection(i);
	}
	
}
int main(){
int user;

//threads para receber mensagens
pthread_t receveThread[MAX_USERS];
for(user=0;user<MAX_USERS;user++){
	if(pthread_create(&receveThread[user], NULL,receve,NULL)) {
	fprintf(stderr, "Error creating thread receve\n");
	}
}
printf("digite o nome da sua maquina\n");
scanf("%s",name);
//threads para receber conexoes
pthread_t connectThread;

if(pthread_create(&connectThread, NULL,connectListen, NULL)) {
	fprintf(stderr, "Error creating thread connect\n");
	}
int ctrl;
//menu principal
while(1){
	printMenu();
	scanf("%d",&ctrl);

	if(ctrl==1){adicionar();}
	if(ctrl==2){enviar();}
	if(ctrl==3){listarContatos();}
	if(ctrl==4){removeContatos();}
	if(ctrl==5){return 0;}
	}
}
