#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#include "FileTransferServer.h"

#define MSG_TRUE "1"
#define MSG_FALSE "0"
#define MSG_DUP "DUPPLICATE"
#define MSG_BLOCKED "BLOCKED"
#define MSG_ERROR "ERROR"

typedef struct roomNode {
	char* roomName;
	char* owner;
	struct roomNode* next;
} Room;

void homepage(int sockfd);
void readRoomInfo(Room* root, FILE* fp);
void createRoom(Room* root, int conn_sock, FILE* db);
void getIntoRoom(Room* root, int conn_sock, FILE* db);
void searchRoom(Room* root, int conn_sock);
void freeRoomList(Room* root);
void fileTransfer(int conn_sock, char* path, int permission);