#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

void *clientHandler(void *);

static int clientFd;

int main(int argc, char* argv[])
{
	int portNumber, socketFd, length;
	int *newSocket;
	int opt = 1;
	struct sockaddr_in socketAddress, clientAddress;
	//first check that the only command line argument is a port number
	//and that the port number is valid
	if(argc < 2)
	{
		cerr << "Syntax is ./server <portNumber>" << endl;
		return 0;
	}
	portNumber = atoi(argv[1]);

	if (portNumber < 2000 || portNumber > 65535)
	{
		cerr << "Please enter a valid port number between 2000 and 65535" << endl;
	}

	//create socket
	if((socketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		cerr << "Cannot open socket" << endl;
		return 0;
	}

	//set socket to allow multiple connections
	if( setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		cerr << "Socket could not allow multiple connections" << endl;
		return 0;
	}

	//bzero((char*) &socketAddress, sizeof(socketAddress));

	//bind socket to given port
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_addr.s_addr = INADDR_ANY;
	socketAddress.sin_port = htons( portNumber );
	if(bind(socketFd, (struct sockaddr*)&socketAddress, sizeof(socketAddress)) < 0)
	{
		cerr << "Cannot bind socket to given port" << endl;
		return 0;
	}

	//Tell the socket to start listening to connections,
	//as specified by Garrison allow a backlog of 20 connections
	if(listen(socketFd, 20) <0 )
	{
		cerr << "Socket can not accept connections";
		return 0;
	}else
	{
		cout << "Waiting for incoming connections...." << endl;
		length = sizeof(struct sockaddr_in);
	}

	//When a client sends a connection requestion to the server, accept it and
	//send it to an individual thread to be dealt with
	while((clientFd = accept(socketFd, (struct sockaddr *)&clientAddress, (socklen_t*)&length)))
	{
		cout << "Connection Accepted" << endl;

		pthread_t masterThread;
		newSocket = (int*) malloc(sizeof *newSocket);
		*newSocket = clientFd;
		if(pthread_create(&masterThread, NULL, clientHandler, NULL) < 0)
		{
			cerr << "Could not create a thread for a client" << endl;
			return 0;
		}
	}
}

void *clientHandler(void *dummyPT)
{
	cout << "in client handler" << endl;
//	//first get the socket descriptor
	int readSize, messageSize;
	char clientMessage[2000];

	//first receive the size of the message
	if(recv(clientFd, &messageSize, sizeof(messageSize),0) < 0)
	{
		cerr << "Could not receive number from server" << endl;
	}
	while((readSize = recv(clientFd,clientMessage, messageSize, 0)) > 0)
	{
		//Hash the message received from the user and send it back to them
		write(clientFd, clientMessage, strlen(clientMessage));
		cout << clientMessage << endl;
	}

	if(readSize == 0)
	{
		cout << "Client Disconnected";
		fflush(stdout);
	}
}
