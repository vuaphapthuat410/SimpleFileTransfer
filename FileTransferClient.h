/*TCP Echo Client*/
#include <stdio.h>          /* These are the usual header files */
#include<stdlib.h> 

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 1024

#define MSG_DUP_FILE "Error: File is existent."
#define MSG_RECV_FILE "Successful transfering."
#define MSG_CLOSE "Cancel file transfer"
#define MSG_RECV "Received."
#define MSG_ACCEPT "Accept"
#define MSG_ERROR "ERROR"

int upload(int client_sock, char* file_path);
int download(int client_sock, char* file_path);
char* extract_file_name(char* file_path);
void clean_and_restore(FILE** fp);
int request_file(int client_sock);
int delete(int client_sock);
int createSubFolder(int client_sock);