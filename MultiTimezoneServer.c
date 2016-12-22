//
//  MultiTimezoneServer
//
//  Created by Hassan Al-ubeidi
//  Username: psyha3
//



// Borrows from the example server and client

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
#define kNUMCLUES 5
#define kDAYTIMEPORT       1313
#define kMULTITIMEZONEPORT 1414

void ServerConnection(int fd);
int getTimezoneNumber(char *input);
char *str_cut(char *input, int length);
void printTimezones(char *input, char *, int);

int main(int argc, const char * argv[])
{
	int server_socket, client_connection;
	struct sockaddr_in server;
	struct sockaddr_in client;
	unsigned int alen;

	printf("Listening for connections on port 1414\n");
	
	server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(kMULTITIMEZONEPORT);
	server.sin_addr.s_addr = INADDR_ANY;
	
	/* bind socket */
	if((bind(server_socket, (struct sockaddr*)&server, sizeof(server)) < 0))
	{
		printf("bind() failed -- %d\n", errno);
		return 1; /* Error */
	}
	
	/* Mark the connection as listening */
	if(listen(server_socket, 15) < 0)
	{
		fprintf(stderr, "Failed to listen()\n");
		return 2;
	}
	
	while(1)
	{
		alen = sizeof(client);
		client_connection = accept(server_socket, (struct sockaddr*)&client, &alen);
		if(client_connection < 0)
		{
			fprintf(stderr, "Accept failed -- %d %d\n", client_connection, errno);
			return 3;
		}
		
		printf("Conenction from %x port %d...\n", ntohl(client.sin_addr.s_addr), ntohs(client.sin_port));
		
		/* Handle connection */
		ServerConnection(client_connection);
		
		close(client_connection);
	}
	
	return 0;
}

void ServerConnection(int fd)
{
	char outBuffer[512];
	char inputBuffer[512];
    char timestr[32];
	ssize_t n;
	time_t current_time;
    char *time_string;
    char *timezone_string;
    int timeZoneNumber;
    int starting;

    time(&current_time);
    time_string = ctime(&current_time);
    time_string[25] = '\0';
	sprintf(outBuffer, "%s-GMT", time_string);

	while(1)
	{
		/* read input */
		n = read(fd, inputBuffer, 1000);
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
            write(fd, outBuffer, strlen(outBuffer));
        }
        else if(timeZoneNumber == 15) {
            sprintf(outBuffer, "BYE\n");
            write(fd, outBuffer, strlen(outBuffer));
        } else if(strcasecmp(str_cut(inputBuffer, 5), "START") == 0) {
        	starting = 1;
        	printTimezones(inputBuffer, outBuffer, fd);
        } else {
        	sprintf(outBuffer, "ERROR\n");
        	write(fd, outBuffer, strlen(outBuffer));
        }
	
		
	}
	
	
}

void printTimezones(char *input, char *outBuffer, int fd) {
	time_t current_time;
    char *time_string;
    int timeZoneNumber;

	int count = atoi(&input[7]);
	int i,j, initial;
	char timezone[6];
	char **time_str_array;

	for(int i=0; i < count; i++) {
		initial = 9 + (4 * i);
		for(int j=0; j<3; j++) {
			timezone[j] = input[initial+j];
		}
		timezone[3] = '\r'; timezone[4] = '\n'; timezone[5]='\0';
		timeZoneNumber = getTimezoneNumber(timezone);
		time(&current_time);
        current_time += (3600 * timeZoneNumber);
        time_string = ctime(&current_time);
        time_string[24] = '-';
        time_string[25] = timezone[0]; time_string[26] = timezone[1]; time_string[27] = timezone[2];
        time_string[28] = '*';
        time_string[29] = '\0';
        write(fd, time_string, strlen(time_string));
	}
}

char *str_cut(char *input, int length) {
	int i;
	char output[length];
	for(i = 0; i<length; i++) {
		output[i] = input[i];
	}
	return output;
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
	} else if(strcasecmp(input, "MSK\r\n") == 0) {
		return 3;
	} else if(strcasecmp(input, "JST\r\n") == 0) {
		return 9;
	} else if(strcasecmp(input, "AEST\r\n") == 0) {
		return 10;
	} else if(strcasecmp(input, "PST") == 0) {
		return -8;
	} else if(strcasecmp(input, "MST") == 0) {
		return -7;
	} else if(strcasecmp(input, "CST") == 0) {
		return -6;
	} else if(strcasecmp(input, "EST") == 0) {
		return -5;
	} else if(strcasecmp(input, "GMT") == 0) {
		return 0;
	} else if(strcasecmp(input, "CET") == 0) {
		return 1;
	} else if(strcasecmp(input, "MSK") == 0) {
		return 3;
	} else if(strcasecmp(input, "JST") == 0) {
		return 9;
	} else if(strcasecmp(input, "AEST") == 0) {
		return 10;
	} else if(strcasecmp(input, "CLOSE\r\n") == 0) {
		return 15; 
	} else return 16; 
	
}
