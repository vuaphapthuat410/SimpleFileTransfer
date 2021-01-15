CFLAGS =-Wall
CC = gcc

server: ServerFuncs.o tcp_server.o FileTransferServer.o room_manage_server.o
	${CC} ${CFLAGS} -lpthread tcp_server.o ServerFuncs.o FileTransferServer.o room_manage_server.o -o server

client: ServerAPI.o  tcp_client.o FileTransferClient.o room_client.o
	${CC} ${CFLAGS} tcp_client.o ServerAPI.o FileTransferClient.o room_client.o -o client

tcp_server.o : tcp_server.c
	${CC} -c ${CFLAGS} -lpthread tcp_server.c

tcp_client.o : tcp_client.c
	${CC} -c ${CFLAGS} tcp_client.c

ServerFuncs.o :ServerFuncs.c 
	${CC} -c ${CFLAGS} ServerFuncs.c 

ServerAPI.o: ServerAPI.c 
	${CC} -c ${CFLAGS} ServerAPI.c 

FileTransferServer.o : FileTransferServer.c
	${CC} -c ${CFLAGS} FileTransferServer.c

FileTransferClient.o: FileTransferClient.c
	${CC} -c ${CFLAGS} FileTransferClient.c
	
room_manage_server.o: room_manage_server.c
	${CC} -c ${CFLAGS} room_manage_server.c

room_client.o: room_client.c
	${CC} -c ${CFLAGS} room_client.c

clean: 
	rm -f *.o *~