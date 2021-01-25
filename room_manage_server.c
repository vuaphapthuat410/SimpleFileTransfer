#include "room_manage_server.h"

// Room function
void readRoomInfo(Room* root, FILE* fp) {
	char roomName[255];
	char owner[255];
	Room* tmp = root;

	fseek(fp, 0, SEEK_SET);
	
	while(!feof(fp)) {
		fscanf(fp, "%s %s\n", roomName, owner);
		Room* newRoom = (Room*) malloc(sizeof(Room));
		newRoom->roomName = strdup(roomName);
		newRoom->owner = strdup(owner);
		newRoom->next = NULL;

		tmp->next = newRoom;
		tmp = tmp->next;
	}
		
	return;
}

void createRoom(Room* root, int conn_sock, FILE* db) { //done
	char roomName[255];
	char owner[255];
	int isDuplicate = 0;
	Room* tmp;

	int bytes_sent, bytes_received;

	// receive room name from client
	while(1) {
		isDuplicate = 0; //reset isDuplicate

		bzero(roomName, 255);
		bytes_received = recv(conn_sock, roomName, 256, 0);
		if(bytes_received <= 0) {
			fprintf(stderr, "Failed to receive room name from client. Try again.\n");
			return;
		}
		else
			roomName[bytes_received] = '\0'; 	
		// receive null or there's already an user
		if(strcmp(roomName, MSG_FALSE) == 0) {
			printf("Cancel create room.\n");
			return;
		}
			

		if(root->next == NULL) {
			fprintf(stderr, "Empty database.\n");
			bytes_sent = send(conn_sock, MSG_ERROR, strlen(MSG_ERROR), 0);
			if (bytes_sent <= 0) {
				fprintf(stderr, "Failed to send signal to client. Try again.\n");
				return;
			}
			return;
		}
			
		tmp = root->next;

		while(tmp != NULL) {
			if(strcmp(tmp->roomName, roomName) == 0) {
				isDuplicate = 1;
				break;
			}
			if(tmp->next == NULL) break;
			tmp = tmp->next; 
		}

		if(isDuplicate) {
			printf("Room existed.\n");
			bytes_sent = send(conn_sock, MSG_DUP, strlen(MSG_DUP), 0);
			if (bytes_sent <= 0) {
				fprintf(stderr, "Failed to send signal to client. Try again.\n");
				return;
			}
		}
		else {
			bytes_sent = send(conn_sock, MSG_TRUE, strlen(MSG_TRUE), 0);
			if (bytes_sent <= 0) {
				fprintf(stderr, "Failed to send signal to client. Try again.\n");
				return;
			}
			break;
		}	
	}

	// receive owner from client
	bzero(owner, 64);
	bytes_received = recv(conn_sock, owner, 255, 0);
	if(bytes_received <= 0) {
		fprintf(stderr, "Failed to receive owner name from client. Try again.\n");
		return;
	}
	else
		owner[bytes_received] = '\0'; 	

	Room* newRoom = (Room*) malloc(sizeof(Room));

	if(newRoom == NULL) {
		perror("\nError: ");
		bytes_sent = send(conn_sock, MSG_ERROR, strlen(MSG_ERROR), 0);
		if (bytes_sent <= 0) {
			fprintf(stderr, "Failed to send signal to client. Try again.\n");
			return;
		}
	}
	else {
		newRoom->roomName = strdup(roomName);
		newRoom->owner = strdup(owner);
		newRoom->next = NULL;
		tmp->next = newRoom; 

		printf("%s\n", newRoom->roomName);
		printf("%s\n", newRoom->owner);

		fseek(db, 0, SEEK_END);
		int bytes_written = fprintf(db, "%s %s\n", roomName, owner);
		fflush(db);
		mkdir(newRoom->roomName, 0700);
		printf("Total bytes written: %d\n", bytes_written);

		bytes_sent = send(conn_sock, MSG_TRUE, strlen(MSG_TRUE), 0);
		if (bytes_sent <= 0) {
			fprintf(stderr, "Failed to send signal to client. Try again.\n");
			return;
		}
	}

	return;
}

void getIntoRoom(Room* root, int conn_sock, FILE* db) {
	char roomName[255];
	char owner[255];
	int bytes_sent, bytes_received;
	//int status;
	Room* tmp;

	bzero(roomName, 255);
	bytes_received = recv(conn_sock, roomName, 256, 0);
	if(bytes_received <= 0) {
		fprintf(stderr, "Failed to receive room name from client. Try again.\n");
		return;
	}
	else 
		roomName[bytes_received] = '\0';

	// receive null or there's already have an user
	if(strcmp(roomName, MSG_FALSE) == 0) {
		fprintf(stderr, "Already get into room.\n");
		return;
	}
		

	if(root->next == NULL) {
		fprintf(stderr, "Empty database.\n");
		bytes_sent = send(conn_sock, MSG_ERROR, strlen(MSG_ERROR), 0);
		if (bytes_sent <= 0) {
			fprintf(stderr, "Failed to send signal to client. Try again.\n");
			return;
		}
		return;
	}
		
	tmp = root->next;

	while(tmp != NULL) {
		if(strcmp(tmp->roomName, roomName) == 0) {
			printf("Found.\n");
			bytes_sent = send(conn_sock, MSG_TRUE, strlen(MSG_TRUE), 0); // inform that roomName is valid
				if (bytes_sent <= 0) {
					fprintf(stderr, "Failed to send signal to client. Try again.\n");
					return;
				}

			bzero(owner, 255);
			bytes_received = recv(conn_sock, owner, 255, 0);
			if(bytes_received <= 0) {
				fprintf(stderr, "Failed to receive owner from client. Try again.\n");
				return;
			}
			else
				owner[bytes_received] = '\0';

			bytes_sent = send(conn_sock, roomName, strlen(roomName), 0); // send logged room name
			if (bytes_sent <= 0) {
				fprintf(stderr, "Failed to send signal to client. Try again.\n");
				return;
			}
			// owner and user are matched.
			if(strcmp(tmp->owner, owner) == 0) { // can delete file
				fprintf(stderr, "Client is owner of this room.\n");
				fileTransfer(conn_sock, roomName, 1);
				return;
			}
			else {
				fprintf(stderr, "Client is not owner of this room.\n");
				fileTransfer(conn_sock, roomName, 0);
				return;
			}
		}
		tmp = tmp->next;		
	}

	printf("Cannot find room.\n");
	bytes_sent = send(conn_sock, MSG_FALSE, strlen(MSG_FALSE), 0);
	if (bytes_sent <= 0) {
		fprintf(stderr, "Failed to send signal to client. Try again.\n");
	}
	return;
}

void searchRoom(Room* root, int conn_sock) {
	char roomName[255];
	Room* tmp;
	int bytes_sent, bytes_received;

	bytes_received = recv(conn_sock, roomName, 256, 0);
	if(bytes_received <= 0) {
		fprintf(stderr, "Failed to get room name. Try again.\n");
		return;
	}
	else 
		roomName[bytes_received] = '\0';
	
	// receive null 
	if(strcmp(roomName, MSG_FALSE) == 0)
		return;

	if(root->next == NULL) {
		fprintf(stderr, "Empty database.\n");
		bytes_sent = send(conn_sock, MSG_ERROR, strlen(MSG_ERROR), 0);
		if (bytes_sent <= 0) {
			fprintf(stderr, "Failed to send signal to client. Try again.\n");
			return;
		}
		return;
	}
		
	tmp = root->next;

	while(tmp != NULL) {
		if(strcmp(tmp->roomName, roomName) == 0) {
			printf("Room is existed.\n");
			bytes_sent = send(conn_sock, MSG_TRUE, strlen(MSG_TRUE), 0);
			if (bytes_sent <= 0) {
				fprintf(stderr, "Failed to send signal to client. Try again.\n");
			}				
			return;
		}
		tmp = tmp->next;
	}

	printf("Cannot find room.\n");
	bytes_sent = send(conn_sock, MSG_ERROR, strlen(MSG_ERROR), 0);
	if (bytes_sent <= 0) {
		fprintf(stderr, "Failed to send signal to client. Try again.\n");
		return;
	}
}

void freeRoomList(Room* root) {
	Room* tmp = root->next;
	Room* guard = tmp;
	if(tmp == NULL) return; //empty linked list
	while(tmp != NULL) {
		guard = tmp->next;
		free(tmp->roomName);
		free(tmp->owner);
		free(tmp);
		tmp = guard;
	}
}

void fileTransfer(int conn_sock, char* path, int permission) {
	char recv_data[BUFF_SIZE];
	int bytes_received;

	//int status;
	int choice;
	
	//start conversation
	do{
		//receives message from client
		memset(recv_data, 0, BUFF_SIZE);
		bytes_received = recv(conn_sock, recv_data, BUFF_SIZE, 0); //blocking
		if (bytes_received <= 0){
			printf("\nConnection closed");
			break;
		}
		printf("%s\n",recv_data);

		choice = atoi(recv_data);
		switch(choice) {
			case 1:
				recv_file(conn_sock, path); //loop receives files from client
				break;
			case 2:
				send_file(conn_sock, path); //loop send files to client
				break;
			case 3:
				delete_file(conn_sock, path, permission);
				break;
			case 4:
				createSubFolder(conn_sock, path);
				break;
			default:
				break;
		}

	} while(choice > 0 && choice < 5);
	
	return;
}