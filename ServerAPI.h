#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // for close <not make warning if close a socket>

#include "room_client.h"

#define BUFF_SIZE 1024

#define MSG_TRUE "1"
#define MSG_FALSE "0"
#define MSG_DUP "DUPPLICATE"
#define MSG_BLOCKED "BLOCKED"
#define MSG_ERROR "ERROR"

void registerAccount(int client_sock, int *status);
void signInAccount(int client_sock, int *status, char **sessionID);
void searchAccount(int client_sock);
void signOutAccount(int client_sock, int *status, char *sessionID);
