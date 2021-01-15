#include "ServerFuncs.h"
#include <pthread.h>

/*
* Receive and echo message to client
* [IN] sockfd: socket descriptor that connects to client 	
*/
typedef struct arg_struct
{
   int connfd;
   Account* root;
   FILE* fp;
} Args;

void *echo(void *arg);

int main(int argc, char *argv[]) {
 
	int listenfd, *connfd;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in *client; /* client's address information */
	int sin_port;
	unsigned int sin_size;
	pthread_t tid;
	
	if(argc < 2) {
		printf("No specified port.\n");
		exit(0);
	}

	Account* root = (Account*)calloc(1, sizeof(Account));

	FILE* db = fopen("account.txt", "r+");
	if (db == NULL)
    {
        fprintf(stderr, "cannot open target file %s\n", argv[1]);
        exit(2);
    }
	readFromFile(root, db);
	
	//Step 1: Construct a TCP socket to listen connection request
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}

	//Step 2: Bind address to socket
	sin_port = atoi(argv[1]);

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(sin_port); 
	server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */   

	if(bind(listenfd,(struct sockaddr*)&server, sizeof(server))==-1){ 
		perror("\nError: ");
		return 0;
	}     

	//Step 3: Listen request from client
	if(listen(listenfd, BACKLOG) == -1){  
		perror("\nError: ");
		return 0;
	}
	
	//Step 4: Communicate with client
	sin_size=sizeof(struct sockaddr_in);
	client = malloc(sin_size);
	while(1){		
		connfd = malloc(sizeof(int));
		if ((*connfd = accept(listenfd, (struct sockaddr *)client, &sin_size)) ==- 1)
			perror("\nError: ");
				
		printf("You got a connection from %s\n", inet_ntoa(client->sin_addr) ); /* prints client's IP */
		// generate an argument for passing to echo
		 Args* arg = (Args*) malloc(sizeof(Args));
		 arg->connfd = *connfd;
		 arg->root = root;
		 arg->fp = db;
		/* For each client, spawns a thread, and the thread handles the new client */
		pthread_create(&tid, NULL, echo, (void*) arg);	
	}
	
	close(listenfd);
	fclose(db);
	return 0;
}

void *echo(void *arg) {
	int sockfd;
	Account* root;
	FILE* db;

	char recv_data[BUFF_SIZE];
	int bytes_received;
	int status;

	sockfd = ((Args*)(arg))->connfd;
	root = ((Args*)(arg))->root;
	db = ((Args*)(arg))->fp;
	free(arg);

	pthread_detach(pthread_self());

	//start conversation
	while(1){
		//receives message from client
		bytes_received = recv(sockfd, recv_data, BUFF_SIZE, 0); //blocking
		if (bytes_received <= 0){
			printf("\nConnection closed");
			break;
		}
		printf("%s\n",recv_data);
		
		switch(atoi(recv_data)) {
			case 1:
				registerAccount(root, sockfd, db);
				break;
			case 2:
				signInAccount(root, sockfd, db, &status);
				if(status == 1) 
					homepage(sockfd);
				break;
			case 3:
				searchAccount(root, sockfd);
				break;
			case 4:
				if(status == 1) signOutAccount(root, sockfd, &status);
				break;
			default: // end process
				if(status == 1) signOutAccount(root, sockfd, &status);
				//freeAccount(root);
				//return 0;
				// close connection then exit thread
				close(sockfd);	
				pthread_exit(NULL);
		}
	}//end conversation

	pthread_exit(NULL);
}