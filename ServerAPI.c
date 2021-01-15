#include"ServerAPI.h"

void registerAccount(int client_sock, int *status) { //done
	char username[255];
	char password[64];
	char buffer[BUFF_SIZE];
	int bytes_sent, bytes_received;

	// if already logged in, cannot registerAccount
	if(*status == 1) {
		fprintf(stderr, "You are already logged in. Cannot register account.\n");
		bytes_sent = send(client_sock, MSG_FALSE, strlen(MSG_FALSE), 0);
		if (bytes_sent <= 0) {
			fprintf(stderr, "Failed to connect to server. Try again.\n");
			return;
		}
		return;
	}

	// send username to server
	while(1) {
		printf("Please enter the username: ");
		fgets(username, 255, stdin);
		username[strlen(username)-1] = '\0';

		// enter nothing, cancel registerAccount
		if(username[0] == '\0') {
			bytes_sent = send(client_sock, MSG_FALSE, strlen(MSG_FALSE), 0);
			if (bytes_sent <= 0) {
				fprintf(stderr, "Failed to connect to server. Try again.\n");
				return;
			}
			return;
		}

		bytes_sent = send(client_sock, username, strlen(username), 0);
		if (bytes_sent <= 0) {
			fprintf(stderr, "Failed to connect to server. Try again.\n");
			return;
		}

		bzero(buffer, BUFF_SIZE);
		bytes_received = recv(client_sock, buffer, BUFF_SIZE, 0);
		if(bytes_received <= 0) {
			fprintf(stderr, "Failed to verify username. Try again.\n");
			return;
		}
		else 
			buffer[bytes_received] = '\0';

		if(strcmp(buffer, MSG_DUP) == 0) {
			fprintf(stderr, "Account existed.\n");
			continue;
		}
		else if(strcmp(buffer, MSG_ERROR) == 0) {
			fprintf(stderr, "Error occurred. Try again.\n");
			return;
		}
		else 
			break;
	}
	
	// send password to server
	printf("Please enter the password: ");
	fgets(password, 64, stdin);
	password[strlen(password)-1] = '\0';

	bytes_sent = send(client_sock, password, strlen(password), 0);
	if (bytes_sent <= 0) {
		fprintf(stderr, "Failed to connect to server. Try again.\n");
		return;
	}

	bzero(buffer, BUFF_SIZE);
	bytes_received = recv(client_sock, buffer, BUFF_SIZE, 0);
	if(bytes_received <= 0) {
		fprintf(stderr, "Failed to verify account. Try again.\n");
		return;
	}
	else 
		buffer[bytes_received] = '\0';

	if(strcmp(buffer, MSG_ERROR) == 0) 
		fprintf(stderr, "Error occurred. Try again.\n");
	else 
		printf("Successful registration.\n");

	return;
}

void signInAccount(int client_sock, int *status, char **sessionID) { // done
	char username[255];
	char password[64];
	char buffer[BUFF_SIZE];
	int bytes_sent, bytes_received;

	if(*status == 1) {
		fprintf(stderr, "You are already logged in.\n");
		bytes_sent = send(client_sock, MSG_FALSE, strlen(MSG_FALSE), 0);
		if (bytes_sent <= 0) {
			fprintf(stderr, "Failed to connect to server. Try again.\n");
			return;
		}
		return;
	}
	// send username to server
	printf("Please enter the username: ");
	fgets(username, 255, stdin);
	username[strlen(username)-1] = '\0';
	// enter nothing, cancel signInAccount
	if(username[0] == '\0') {
		bytes_sent = send(client_sock, MSG_FALSE, strlen(MSG_FALSE), 0);
		if (bytes_sent <= 0) {
			fprintf(stderr, "Failed to connect to server. Try again.\n");
			return;
		}
		return;
	}

	bytes_sent = send(client_sock, username, strlen(username), 0);
	if (bytes_sent <= 0) {
		fprintf(stderr, "Failed to connect to server. Try again.\n");
		return;
	}

	bzero(buffer, BUFF_SIZE);
	bytes_received = recv(client_sock, buffer, BUFF_SIZE, 0);
	if(bytes_received <= 0) {
		fprintf(stderr, "Failed to verify username. Try again.\n");
		return;
	}
	else
		buffer[bytes_received] = '\0';

	if(strcmp(buffer, MSG_FALSE) == 0) { // account not found
		fprintf(stderr, "Account not found. Try again.\n");
		return;
	}
	else if(strcmp(buffer, MSG_ERROR) == 0) {	// Other error occurred
		fprintf(stderr, "Error occurred. Try again.\n");
		return;
	}
	else if(strcmp(buffer, MSG_BLOCKED) == 0){
		fprintf(stderr, "Account was blocked.Please contact your system administrator.\n");
		return;
	}

	for(int index = 0; index < 3; index++) {
		printf("Please enter the password: ");
		fgets(password, 64, stdin);
		password[strlen(password)-1] = '\0';

		bytes_sent = send(client_sock, password, strlen(password), 0);
		if (bytes_sent <= 0) {
			fprintf(stderr, "Failed to connect to server. Try again.\n");
			return;
		}

		bzero(buffer, BUFF_SIZE);
		bytes_received = recv(client_sock, buffer, BUFF_SIZE, 0);
		if(bytes_received <= 0) {
			fprintf(stderr, "Failed to verify account. Try again.\n");
			return;
		}
		else
			buffer[bytes_received] = '\0';

		if(strcmp(buffer, MSG_FALSE) == 0) {
			fprintf(stderr, "Password is incorrect.\n");
		}
		else {
			printf("Hello %s\n", buffer);
			*status = 1;
			// sessionID will be kept at sessionID
			*sessionID = strdup(buffer);
			return; //break loop
		}
	}
	// Wrong password more than 3 times. Block account.
	printf("Account \"%s\" is blocked.\n", username);
	return;
}

void searchAccount(int client_sock) { //done 
	char username[255];
	char buffer[BUFF_SIZE];
	int bytes_sent, bytes_received;

	printf("Please enter the username: ");
	fgets(username, 255, stdin);
	username[strlen(username)-1] = '\0';
	// enter nothing, cancel searchAccount
	if(username[0] == '\0') {
		bytes_sent = send(client_sock, MSG_FALSE, strlen(MSG_FALSE), 0);
		if (bytes_sent <= 0) {
			fprintf(stderr, "Failed to connect to server. Try again.\n");
			return;
		}
		return;
	}

	bytes_sent = send(client_sock, username, strlen(username), 0);
	if (bytes_sent <= 0) {
		fprintf(stderr, "Failed to connect to server. Try again.\n");
		return;
	}

	bytes_received = recv(client_sock, buffer, BUFF_SIZE, 0);
	if(bytes_received <= 0) {
		fprintf(stderr, "Failed to verify username. Try again.\n");
		return;
	}
	else
		buffer[bytes_received] = '\0';

	if(strcmp(buffer, MSG_TRUE) == 0) // return true mean user found and active
		printf("Account is active\n");
	else if(strcmp(buffer, MSG_FALSE) == 0) //return false mean user found and not active
		printf("Account is blocked\n");
	else 
		printf("Cannot find account.\n");

	return;
}

void signOutAccount(int client_sock, int *status, char *sessionID) { // done
	char buffer[BUFF_SIZE];
	int bytes_sent, bytes_received;

	if(*status == 0) {
		fprintf(stderr, "Account is not signed in.\n");
		bytes_sent = send(client_sock, MSG_FALSE, strlen(MSG_FALSE), 0);
		if (bytes_sent <= 0) {
			fprintf(stderr, "Failed to signed out. Try again.\n");
			return;
		}
		return;
	}

	bytes_sent = send(client_sock, sessionID, strlen(sessionID), 0);
	if (bytes_sent <= 0) {
		fprintf(stderr, "Failed to signed out. Try again.\n");
		return;
	}

	bzero(buffer, BUFF_SIZE);
	bytes_received = recv(client_sock, buffer, BUFF_SIZE, 0);
	if(bytes_received <= 0) {
		fprintf(stderr, "No message from server. Connection lost.\n");
		return;
	}
	else
		buffer[bytes_received] = '\0';

	if(strcmp(buffer, MSG_TRUE) == 0) {
		printf("Goodbye %s.\n", sessionID);
		free(sessionID); // free sessionID
		*status = 0;
	}
	else{
		fprintf(stderr, "Error occurred. Try again.\n");
	}
		
	return;
}

int homepage(int client_sock, char* sessionID) {
	int bytes_sent;
	//char buff[BUFF_SIZE];

	int choice;
	char str_choice[2]; //change size if needed
	int cache;
	
	int status;
	char *roomID = (char *)malloc(255);

	//send message
	do{
		printf("Room management\n");
		printf("---------------------------------------------\n");
		printf("	1. Create room\n");
		printf("	2. Get into room\n");
		printf("	3. Search for room\n");
		printf("Your choice (1-3) other to quit):\n");
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
				createRoom(client_sock, sessionID);
				break;
			case 2:
				getIntoRoom(client_sock, sessionID, &status, &roomID);
				if(status == 1) {
					fileTransfer(client_sock);
					status = 0; // after fileTransfer, set status to 0 (log out)
					free(roomID); // free roomID
				}		
				else
					printf("No room specified. Try again.\n");

				break;
			case 3:
				searchRoom(client_sock);
				break;
			default: 
				return 0;
		}

	} while(choice > 0 && choice < 4);
	
	free(roomID);
	return 0;
}






