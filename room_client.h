#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // for close <not make warning if close a socket>

#define MSG_TRUE "1"
#define MSG_FALSE "0"
#define MSG_DUP "DUPPLICATE"
#define MSG_BLOCKED "BLOCKED"
#define MSG_ERROR "ERROR"

#include "FileTransferClient.h"

int homepage(int client_sock, char* sessionID);
void createRoom(int client_sock, char* owner);
void getIntoRoom(int client_sock, char* owner, int *status, char **sessionID);
void searchRoom(int client_sock);
void fileTransfer(int client_sock);