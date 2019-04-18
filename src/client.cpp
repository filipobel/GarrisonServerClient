#include <iostream>
#include <iomanip>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

using namespace std;

void getMessage(char* message);
void convertSHA1BinaryToCharStr(const unsigned char * const hashbin, char * const hashstr);

int main(int argc, char *argv[])
{
	int socketFd, portNumber;
	struct sockaddr_in serverAddress;
	char* initialMessage;
	//first check that a port is given
	if(argc != 2)
	{
		cerr << "Syntax is : ./client <port>" <<endl;
	}

	portNumber = atoi(argv[1]);

	if (portNumber < 2000 || portNumber > 65535)
	{
		cerr << "Please enter a valid port number between 2000 and 65535" << endl;
		return 0;
	}

	//create client socket
	if((socketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		cerr << "Cannot open socket" << endl;
		return 0;
	}
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); //this is the address for localhost.
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(portNumber);

	//connect to the server
	if(connect(socketFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
	{
		cerr << "Could not connect to the server";
		return 0;
	}

	while(true)
	{
		initialMessage = (char*) calloc(1,sizeof(char));
		cout << "Type you message to send to the server. Type q to end connection to the server" << endl;
		getMessage(initialMessage);

		char message[strlen(initialMessage)], serverReply[strlen(initialMessage)];


		// copying the contents of the
		// string to char array
		strcpy(message, initialMessage);
		if(message[0] == 'q' && strlen(message) == 1)
		{
			cout << "Close client" << endl;
			close(socketFd);
			return 0;
		}else
		{
			int size = strlen(message);
			//first send across the size of the message
			if(send(socketFd,&size, sizeof(size), 0) < 0)
			{
				cerr << "Could not send size of message";
				return 0;
			}
			if(send(socketFd, message, size,0) < 0)
			{
				cerr << "Could not send message";
				return 0;
			}

			//Receive the hashed message back from the server
			if(recv(socketFd, serverReply, 20, 0) > 0) //this is 20 as a SHA-1 algorithm produces a 20 byte long array
			{
				char hashstr[41];
				convertSHA1BinaryToCharStr((unsigned char *)serverReply, hashstr);
				cout << "Hashed reply from server: "<< hashstr <<endl;
			}
		}
	}
	close(socketFd);
	return 1;
}

void getMessage(char* message)
{
	char nextChar;
	int leng;
	while(scanf("%c", &nextChar)==1)
	{
	  if(nextChar== '\n')
	     break;
	   leng = strlen(message);
	   message= (char*) realloc(message,leng+1);
	   *(message+leng) = nextChar;
	   *(message+leng+1) = '\0';
	}
}

void convertSHA1BinaryToCharStr(const unsigned char * const hashbin, char * const hashstr)
{
	for(int i = 0; i<20; ++i)
	{
		sprintf(&hashstr[i*2], "%02X", hashbin[i]);
	}
}
