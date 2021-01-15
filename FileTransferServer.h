/*TCP Echo Server*/
#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define BACKLOG 20   /* Number of allowed connections */
#define BUFF_SIZE 1024

#define MSG_DUP_FILE "Error: File is existent."
#define MSG_RECV_FILE "Successful transfering."
#define MSG_CLOSE "Cancel file transfer"
#define MSG_RECV "Received."
#define MSG_ACCEPT "Accept"
#define MSG_ERROR "ERROR"

int recv_file(int conn_sock, char dir_name[]);
int send_file(int conn_sock, char dir_name[]);
char* extract_file_name(char* file_path);
char* get_file_path(int conn_sock, char* dir_name);
int delete_file(int conn_sock, char* dir_name, int permission);
int createSubFolder(int conn_sock, char* dir_name);