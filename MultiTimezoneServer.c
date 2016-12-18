//
//  main.c
//  KnockKnockServer
//
//  Created by Steven Bagley on 12/11/2015.
//  Copyright © 2015 Steven Bagley. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <time.h>

#define kBufSize 12
#define WAITING 0
#define SENTKNOCKKNOCK 1
#define SENTCLUE 2
#define ANOTHER 3
#define BYE 4

char *clues[] = { "Turnip", "Little Old Lady", "Atch", "Who", "Who" };
char *answers[] = { "Turnip the heat, it's cold in here!",
	"I didn't know you could yodel!",
	"Bless you!",
	"Is there an owl in here?",
	"Is there an echo in here?" };
#define kNUMCLUES 5

void ServerConnection(int fd);
int getTimezone(char *input);

int main(int argc, const char * argv[])
{
	int serverSocket, clientConnection;
	struct sockaddr_in server;
	struct sockaddr_in client;
	unsigned int alen;

	printf("Listening for connections on port 4444\n");
	
	serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(4444);
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
		
		close(clientConnection);
	}
	
	return 0;
}

void ServerConnection(int fd)
{
	char outBuffer[512];
	char inputBuffer[512];
	int state = WAITING;
	int joke = 0;
    char timestr[32];
	ssize_t n;
	time_t current_time;
    char *time_string;
    char *timezone_string;
    int timeZoneNumber;

    time(&current_time);
    time_string = ctime(&current_time);
    // time_string[24] = '\0';
	sprintf(outBuffer, "%s-GMT", time_string);
	write(fd, outBuffer, strlen(outBuffer));
	state = SENTKNOCKKNOCK;

	while(state != BYE)
	{
		/* read input */
		n = read(fd, inputBuffer, 510);
		if(n <= 0)
		{
			fprintf(stderr, "read error\n");
			return;
		}
		inputBuffer[n] = 0;
        timeZoneNumber = getTimezoneNumber(inputBuffer);
        if(timeZoneNumber < 15) {
            time(&current_time);
            current_time += (3600 * timeZoneNumber);
            time_string = ctime(&current_time);
            time_string[24] = '\0';
            sprintf(outBuffer, "%s-%s", time_string, inputBuffer);
        } else {
            sprintf(outBuffer, "ERROR\n");
        }
		
        
		// if(0 == strcasecmp(inputBuffer, "PST\n"))
        // {
        //     sprintf(outBuffer, "FUCK\r\n");
        //     state = SENTCLUE;
        // }
	
		/* Write the output */
		write(fd, outBuffer, strlen(outBuffer));
	}
	
	
}


int getTimezoneNumber(char *input) {
	if(strcasecmp(input, "PST\r\n") == 0) {
		return -8;
	} else if(strcasecmp(input, "MST\r\n") == 0) {
		return -7;
	} else if(strcasecmp(input, "CST\r\n") == 0) {
		return -6;
	} else if(strcasecmp(input, "EST\r\n") == 0) {
		return -5;
	} else if(strcasecmp(input, "GMT\r\n") == 0) {
		return 0;
	} else if(strcasecmp(input, "CET\r\n") == 0) {
		return 1;
	} else if(strcasecmp(input, "MSK") == 0) {
		return 3;
	} else if(strcasecmp(input, "JST\r\n") == 0) {
		return 9;
	} else if(strcasecmp(input, "AEST\r\n") == 0) {
		return 10;
	} else return 15; 
}
