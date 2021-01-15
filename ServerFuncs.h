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

#include "room_manage_server.h"

#define BACKLOG 20   /* Number of allowed connections */
#define BUFF_SIZE 1024

#define ACTIVE 1
#define BLOCKED 0

#define MSG_TRUE "1"
#define MSG_FALSE "0"
#define MSG_DUP "DUPPLICATE"
#define MSG_BLOCKED "BLOCKED"
#define MSG_ERROR "ERROR"

typedef struct accountSaveDataLinkedList {
	char* username;
	char* password;
	int status;
	struct accountSaveDataLinkedList* next;
} Account;

void readFromFile(Account* root, FILE* fp);
void registerAccount(Account* root, int client_sock, FILE* db);
void signInAccount(Account* root, int client_sock, FILE* db, int* isLoggedIn);
void searchAccount(Account* root, int client_sock);
void signOutAccount(Account* root, int client_sock, int* isLoggedIn);
void printFile(FILE* fp);
void freeAccount(Account* root);
