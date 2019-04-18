#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/sha.h>

using namespace std;

void *clientHandler(void *);

static int clientFd;

int main(int argc, char* argv[])
{
	int portNumber, socketFd, length;
	int opt = 1;
	struct sockaddr_in socketAddress, clientAddress;

	pthread_t masterThread[20];
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

	int numberThreads = 0;
	//When a client sends a connection requestion to the server, accept it and
	//send it to an individual thread to be dealt with
	while(numberThreads < 20)
	{
		clientFd = accept(socketFd, (struct sockaddr *)&clientAddress, (socklen_t*)&length);
		if(clientFd < 0)
		{
			cerr << "Connections was not accepted";
			return 0;
		}
		cout << "Connection Accepted" << endl;


		if(pthread_create(&masterThread[numberThreads], NULL, clientHandler, NULL) < 0)
		{
			cerr << "Could not create a thread for a client" << endl;
			return 0;
		}

		numberThreads++;
	}
	for(int i = 0; i<20; i++)
	{
		pthread_join(masterThread[i],NULL);
	}
}

void *clientHandler(void *dummyPT)
{
//	//first get the socket descriptor
	int readSize, messageSize;

	//first receive the size of the message
	if(recv(clientFd, &messageSize, sizeof(messageSize),0) < 0)
	{
		cerr << "Could not receive number from server" << endl;
	}
	char clientMessage[messageSize];
	while((readSize = recv(clientFd,clientMessage, messageSize, 0)) > 0)
	{
		//Hash the message received from the user and send it back to them
	    unsigned char hash[20];

	    // compute the sha1 of the input, and store it our  hash array
	    SHA1((unsigned char*)clientMessage, messageSize, hash);
	    for(int i= 0; i<1000; i++)
	    {
	    	SHA1(hash, messageSize, hash);
	    }
		send(clientFd, hash, 20,0);
		fflush(stdout);

	}

	if(readSize == 0)
	{
		cout << "Client Disconnected";
		fflush(stdout);
	}

	close(clientFd);
}
