#include "ServerAPI.h"

int status = 0; // initialize status
char *sessionID = NULL;

int main(int argc, char *argv[]) {
	int client_sock;
	int SERVER_PORT; 
	char* SERVER_ADDR;
	struct sockaddr_in server_addr; /* server's address information */
	int bytes_sent;

	int choice;
	char str_choice[2]; //change size if needed
	int cache;
	
	if(argc < 3) {
		printf("No specified port or ip address.\n");
		exit(0);
	}

	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	SERVER_ADDR = argv[1];
	SERVER_PORT = atoi(argv[2]);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("Error!Can not connect to sever! Client exit imediately!\n");
		return 0;
	}
		
	//Step 4: Communicate with server			
	//send message
	do{
		printf("USER MANAGEMENT PROGRAM\n");
		printf("---------------------------------------------\n");
		printf("	1. Register\n");
		printf("	2. Sign in\n");
		printf("	3. Search\n");
		printf("	4. Sign out\n");
		printf("Your choice (1-4, other to quit):\n");
		scanf("%d", &choice);
		while((cache = getchar()) != '\n' && cache != EOF);

		sprintf(str_choice,"%d",choice);
		bytes_sent = send(client_sock, str_choice, 2, 0); // change buffer size if choice size changes
		if (bytes_sent <= 0) {
			fprintf(stderr, "Failed to send request to server. Try again.\n");
			continue;
		}

		switch(choice) {
			case 1:
				registerAccount(client_sock, &status);
				break;
			case 2:
				signInAccount(client_sock, &status, &sessionID);
				if(status == 1)
					homepage(client_sock, sessionID);
				break;
			case 3:
				searchAccount(client_sock);
				break;
			case 4:
				if(status == 1) signOutAccount(client_sock, &status, sessionID); // if logged in, sign out
				break;
			default: 
				if(status == 1) signOutAccount(client_sock, &status, sessionID); // if logged in, sign out
				return 0;
		}

	} while(choice > 0 && choice < 5);
	
	//Step 4: Close socket
	close(client_sock);
	return 0;
}
