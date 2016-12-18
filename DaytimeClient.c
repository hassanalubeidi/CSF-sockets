//
//  DaytimeClient
//
//  Created by Hassan Al-ubeidi
//  Username: psyha3
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
#define kBufSize 36

int main(int argc, const char * argv[])
{
   char buf[kBufSize+1];
    char line[128];
    ssize_t n;
    int i;
    int more;
    
    int sockFD;
    int port;
    struct sockaddr_in sad;
    struct hostent *ptrh;
    
    sockFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    memset(&sad, 0, sizeof(sad));                       /* Sets every byte of the address to be zero */
    sad.sin_family = AF_INET;
    
    /* Look up name */
    ptrh = gethostbyname(argv[1]);
    memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);
    
    /* Set port */
    sscanf(argv[2], "%d", &port);
    sad.sin_port = htons(port);
    connect(sockFD, (struct sockaddr *)&sad, sizeof(sad));
    n = read(sockFD, buf, kBufSize);

            
    printf("%s", buf);
    
    close(sockFD);
    
    return 0;
}
