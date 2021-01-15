#include "FileTransferClient.h"

int upload(int client_sock, char* file_path) {
	struct stat st;
	char buff[BUFF_SIZE];
	char sendbuff[BUFF_SIZE];
	int bytes_sent, bytes_received;
	FILE* fp;
	int nLeft, idx;
	char* file_name = NULL;
	off_t file_size = 0;
	char file_size_str[65];
	size_t result = 0;

	if(file_path[0] == '\0') { // enter an empty string
		printf("Sending file ended. Exiting.\n");
		bytes_sent = send(client_sock, file_path, 1, 0);
		if(bytes_sent < 0){
			perror("\nError: ");
			return 0;
		}
		return 1;
	}

	// check if file exists
	if(stat(file_path, &st) == -1) { // Not exists
		fprintf(stderr, "Error: File not found.\n");
		bytes_sent = send(client_sock, MSG_CLOSE, strlen(MSG_CLOSE), 0);
		if(bytes_sent < 0){
			perror("\nError: ");
			return 0;
		}
		return -1;
	}

	file_name = extract_file_name(file_path);
	printf("Uploading file to server: %s\n",file_name);	
	bytes_sent = send(client_sock, file_name, strlen(file_name), 0);
	if(bytes_sent < 0){
		perror("\nError: ");
		return -1;
	}
	
	// confirm that server received file name and check file status on server side
	bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
	if(bytes_received < 0){
		perror("Error: ");
		return 0;
	}
	else
		buff[bytes_received] = '\0';

	if(strcmp(buff, MSG_DUP_FILE) == 0) {	//file was found on server, duplicate file	
		printf("%s\n", buff);
		return -1;
	}		
		
	bzero(buff, sizeof(buff));

	file_size = st.st_size;
	sprintf(file_size_str,"%lu",file_size);
	bytes_sent = send(client_sock, file_size_str, strlen(file_size_str), 0);
	if(bytes_sent < 0){
		perror("\nError: ");
		return -1;
	}

	//open file and send data
	if((fp=fopen(file_path, "rb")) == NULL) { // need send error message
		fprintf(stderr, "Open file error.\n");
		exit(1);
	}
	int loop_size = file_size;
	nLeft = file_size%BUFF_SIZE;	// cuz file size is not divisible by BUFF_SIZE

	while(loop_size > 0) {
		idx = 0;

		result += fread(sendbuff, 1, nLeft, fp); // use fread instead of fgets because fgets stop reading if newline is read
		while (nLeft > 0)
		{
			// Assume s is a valid, connected stream socket
			bytes_sent = send(client_sock, &sendbuff[idx], nLeft, 0);
			if (bytes_sent <= 0)
			{
				// Error handler
				printf("Connection closed.Trying again.\n");
			}
			nLeft -= bytes_sent;
			idx += bytes_sent;
		}
		
		bzero(sendbuff, sizeof(sendbuff)); 
		loop_size -= BUFF_SIZE; // decrease unfinished bytes 
		nLeft = BUFF_SIZE;		// reset nLeft
	}

	if(result != file_size) {
		printf("Error reading file.\n");
		return -1;
	}

	// clean
	fclose(fp);
	free(file_name);
	return 0;
}

int download(int client_sock, char* file_path) {
	struct stat st;
	int size_file;
	char recv_data[BUFF_SIZE];
	int bytes_sent, bytes_received;
	char *file_name = (char *)malloc(512);
	char* tok;
	FILE* fp = NULL;
	int nLeft, idx;
	int status;

	// check if directory exists, if not mkdir
	if(stat(file_path, &st) == -1) {
		char *tmp_path = (char *)malloc(sizeof(char)*512);
		getcwd(tmp_path, 512);
		char* folder = strdup(file_path);
		tok = strtok(folder, "/");
		while(tok != NULL) {
			mkdir(tok, 0700); //config permissions by changing 2nd argument
			chdir(tok);    // chdir ~ cd 
			tok = strtok(NULL, "/");
		}
		chdir(tmp_path);
		free(tmp_path);
		free(folder);
	}

	// choose file from server
	status = request_file(client_sock);
	if(status == -1) {
		printf("Error occurred while requesting file from server.\n");
		return -1;
	}
	else if (status == 1) {
		printf("Transfer completed successfully. Close the connection.\n");
		return 1;
	}
	//receives file name																
	bytes_received = recv(client_sock, recv_data, BUFF_SIZE - 1, 0);
	if(bytes_received < 0){
		perror("Error: ");
		return -1;	//meet error, aborted
	}
	else
		recv_data[bytes_received] = '\0';								// check with server send format

	if(recv_data[0] == '\0') {
		printf("Receiving data from server end. Exiting.\n");	
		return 1;
	}

	if(strcmp(recv_data, MSG_CLOSE) == 0) { //file not found on server 			
		printf("You enter wrong file name or file has been deleted.\n");
		return -1;
	}			

	// check if file exists, if not create new one, else return error
	// already at destination folder
	// echo to server 
	strcpy(file_name, file_path);
	strcat(file_name, "/");
	strcat(file_name, recv_data);

	if(stat(file_name, &st) == -1) {// file does not exist
		fp = fopen(file_name, "wb");
		if(fp == NULL) {
			printf("File path error\n");	
			fclose(fp); return -1;
		}
		bytes_sent = send(client_sock, MSG_RECV, strlen(MSG_RECV), 0); //echo that received file name and no duplicate file on client
		if(bytes_sent < 0){
			perror("\nError: ");
			fclose(fp); return -1;	//meet error, aborted
		}
	}
	else {
		printf("Duplicate file.\n");
		bytes_sent = send(client_sock, MSG_DUP_FILE, strlen(MSG_DUP_FILE), 0); //echo that received file name and duplicate file on client
		if(bytes_sent < 0){
			perror("\nError: ");
		}		
		return -1;
	}
	

	printf("File name: %s\n", recv_data);
	bzero(recv_data, bytes_received); //empty buffer

	//receives file size
	bytes_received = recv(client_sock, recv_data, BUFF_SIZE - 1, 0);
	if(bytes_received < 0){
		perror("Error: ");
		fclose(fp); 
		return -1;	//meet error, aborted
	}
	else
		recv_data[bytes_received] = '\0';																// check with client send format

	size_file = atoi(recv_data); 

	printf("File size: %s\n", recv_data);
	bzero(recv_data, bytes_received); //empty buffer

	nLeft = size_file%BUFF_SIZE;	// cuz file size is not divisible by BUFF_SIZE
	int loop_size = size_file;
	
	while(loop_size > 0) {
		idx = 0;			// reset idx

		while (nLeft > 0)
		{
			bytes_received = recv(client_sock, &recv_data[idx], nLeft, 0); // read at missing data index
			if (bytes_received <= 0)
			{
				// Error handler
				printf("Connection closed. Trying again.\n");
			}
			idx += bytes_received; // if larger then socket size
			nLeft -= bytes_received; 
		}

		fwrite(recv_data, 1, idx, fp); //idx is the real length of recv_data
		bzero(recv_data, sizeof(recv_data)); 
		loop_size -= BUFF_SIZE; // decrease unfinished bytes   		- 
		nLeft = BUFF_SIZE;	// reset nLeft
	}

	// sucessful block
	fclose(fp);
	return 0; 
}

int delete(int client_sock) {
	char recv_data[BUFF_SIZE];
	int bytes_received;
	int status;

	bytes_received = recv(client_sock, recv_data, BUFF_SIZE - 1, 0);
	if(bytes_received < 0){
		perror("Error: ");
		return -1;	//meet error, aborted
	}
	else
		recv_data[bytes_received] = '\0';								// check with server send format

	if(strcmp(recv_data, MSG_CLOSE) == 0) { // permissions on server		
		printf("You don't have permission to delete data from server.\n");
		return -1;
	}

	// choose file from server
	printf("Choose file from server:\n");
	status = request_file(client_sock);
	if(status == -1) {
		printf("Error occurred while requesting file from server.\n");
		return -1;
	}
	else if (status == 1) {
		printf("Cancel file delete. Close the connection.\n");
		return 1;
	}
	//receives file name		
	printf("Getting file status from server....\n");														
	bytes_received = recv(client_sock, recv_data, BUFF_SIZE - 1, 0);
	if(bytes_received < 0){
		perror("Error: ");
		return -1;	//meet error, aborted
	}
	else
		recv_data[bytes_received] = '\0';								// check with server send format

	if(recv_data[0] == '\0') {
		printf("Receiving data from server end. Exiting.\n");	
		return 1;
	}

	if(strcmp(recv_data, MSG_CLOSE) == 0) { //file not found on server 			
		printf("You enter wrong file name or file has been deleted.\n");
		return -1;
	}

	if(strcmp(recv_data, MSG_ERROR) == 0) { //file has not been deleted on server 			
		printf("File has not been deleted on server.\n");
		return -1;
	}			

	printf("File: %s has been deleted.\n", recv_data);

	// sucessful block
	return 0; 
}

int createSubFolder(int client_sock) {
	char recv_data[BUFF_SIZE];
	int bytes_sent, bytes_received;
	int status;
	char folder[255];

	// choose file from server
	printf("Choose folder from server:\n");
	status = request_file(client_sock);
	if(status == -1) {
		printf("Error occurred while get folder status from server.\n");
		return -1;
	}
	else if (status == 1) {
		printf("Cancel mkdir. Close the connection.\n");
		return 1;
	}

	printf("Please enter the folder name: ");
	fgets(folder, 255, stdin);
	folder[strlen(folder)-1] = '\0';

	bytes_sent = send(client_sock, folder, strlen(folder), 0);
	if(bytes_sent < 0){
		perror("\nError: ");
		return -1;
	}

	bytes_received = recv(client_sock, recv_data, BUFF_SIZE - 1, 0);
	if(bytes_received < 0){
		perror("Error: ");
		return -1;	//meet error, aborted
	}
	else
		recv_data[bytes_received] = '\0';

	if(strcmp(recv_data, MSG_ERROR) == 0) { //file has not been deleted on server 			
		printf("Folder has not been created on server.\n");
		return -1;
	}

	printf("Folder: %s has been created.\n", recv_data);

	// sucessful block
	return 0; 
}

char* extract_file_name(char* file_path) {
	int i;
	int n = strlen(file_path);
	char* file_name;
	for(i = n-1; i >= 0; --i) {
		if(file_path[i] == '/')
			break;
	}

	if(i == 0) //current directory so that no '/'
		return file_path;

	file_name = (char*)malloc((n-i)*sizeof(char));
	memcpy(file_name, &file_path[i+1], n-i);

	return file_name;
}

int request_file(int client_sock) {
	// for sending signal
    char buff[BUFF_SIZE];
    int bytes_sent, bytes_received;
    char file_name[128];

    while(1) {
    	// check if server is already open for file transfer
    	bzero(buff, sizeof(buff));
	    bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
		if(bytes_received < 0){
			perror("Error: ");
			return -1;
		}

		if(strcmp(buff, MSG_ACCEPT) != 0) {
			printf("%s\n", buff);
			return -1;
		}
		// receive list of files
		bzero(buff, sizeof(buff));
		bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
		if(bytes_received < 0){
			perror("Error: ");
			return -1;
		}

		// user choose file/folder
		printf("%s\n", buff);
        printf("Choose a file/folder: ");
        fgets(file_name, 128, stdin);
        file_name[strlen(file_name)-1] = '\0';

        if(file_name[0] == '\0') {
        	bytes_sent = send(client_sock, MSG_CLOSE, strlen(MSG_CLOSE), 0);
        	printf("You cancelled file transfer!\n");
			if(bytes_sent < 0){
				perror("\nError: ");
				return -1;
			}
			return 1;
        }

       	bytes_sent = send(client_sock, file_name, strlen(file_name), 0);
		if(bytes_sent < 0){
			perror("\nError: ");
			return -1;
		}


		bzero(buff, sizeof(buff));
		bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
		if(bytes_received < 0){
			perror("Error: ");
			return -1;
		}
        
        if(strcmp(buff, MSG_ACCEPT) != 0) {
			printf("%s\n", buff);
			continue;
		}
		else 
			return 0;
    }
}

