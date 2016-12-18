//
//  DaytimeServer
//
//  Created by <insert name>
//  Username: <inser username>
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <time.h>

#define kDAYTIMEPORT       1313
#define kMULTITIMEZONEPORT 1414


#define kBufSize 12
#define WAITING 0
#define SENTKNOCKKNOCK 1
#define SENTCLUE 2
#define ANOTHER 3
#define BYE 4
#define kNUMCLUES 5

void ServerConnection(int fd);

int main(int argc, const char * argv[])
{
    int serverSocket, clientConnection;
	struct sockaddr_in server;
	struct sockaddr_in client;
	unsigned int alen;

	printf("Listening for connections on port %d\n", kDAYTIMEPORT);
	
	serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(kDAYTIMEPORT);
	server.sin_addr.s_addr = INADDR_ANY;
	
	/* bind socket */
	if((bind(serverSocket, (struct sockaddr*)&server, sizeof(server)) < 0))
	{
		printf("bind() failed -- %d\n", errno);
		return 1; /* Error */
	}
	
	/* Mark the connection as listening */
	if(listen(serverSocket, 15) < 0)
	{
		fprintf(stderr, "Failed to listen()\n");
		return 2;
	}
	
	while(1)
	{
		alen = sizeof(client);
		clientConnection = accept(serverSocket, (struct sockaddr*)&client, &alen);
		if(clientConnection < 0)
		{
			fprintf(stderr, "Accept failed -- %d %d\n", clientConnection, errno);
			return 3;
		}
		
		printf("Conenction from %x port %d...\n", ntohl(client.sin_addr.s_addr), ntohs(client.sin_port));
		
		/* Handle connection */
		ServerConnection(clientConnection);
		
		// close(clientConnection);
	}
	
	return 0;
}

void ServerConnection(int fd)
{
	char outBuffer[512];
	char inputBuffer[512];
	char timeinfo_string[32];
	int state = WAITING;
	ssize_t n;
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	strcpy(timeinfo_string, asctime (timeinfo));
	timeinfo_string[24] = '\0';
	sprintf(outBuffer, "%s-GMT\n", timeinfo_string);
	write(fd, outBuffer, strlen(outBuffer));
}

