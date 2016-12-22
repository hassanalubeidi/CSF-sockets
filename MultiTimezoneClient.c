//
//  MultiTimezoneClient
//
//  Created by Hassan Al-ubeidi
//  Username: psyha3
//

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

// Borrows from the example server and client

#define kBufSize 12


int main(int argc, const char * argv[])
{
    char buf[kBufSize+1];
    char line[2000];
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
    sscanf("1414", "%d", &port);
    sad.sin_port = htons(port);
    connect(sockFD, (struct sockaddr *)&sad, sizeof(sad));



    char start_string[8+(argc * 4)];
    strcpy(start_string, "START: ");
    start_string[7] = (argc -2) + '0';
    int j, start_int = 8;

    for(i = 2; i < argc; i++) {
        start_string[start_int] = ' ';
        for(j=1; j<4; j++) {
            start_string[start_int + j] = argv[i][j - 1];
        }
        start_int += 4;
    }
        printf("%s\n",start_string);
    write(sockFD, start_string, strlen(start_string)); 
    int x =0, starting = 1;
    do {
        /* Read input line */
        do {
            more = 1;
            n = read(sockFD, buf, kBufSize);
            buf[n] = '\0';
            
            if(n <= 0)
                break;

            for(i = 0; i < n; i++) {
                x++;
                if(buf[i] == 10) {
                    more = 0;
                    break;
                }
                if(buf[i] == '*') {
                    buf[i] = '\n';
                }

            }
            if(x > 57 && starting == 1) { more = 0; starting = 0; } 
            
            printf("%s", buf);
        } while(more);
        
        if(n <= 0) {

            break;
        }
        fgets(line, 120, stdin);
        line[strlen(line)-1] = 0;
        if(strcasecmp(line, "CLOSE")==0) {
            n -= 1000;
            printf("BYE\r\n");
        }
        strcat(line, "\r\n");
        
        write(sockFD, line, strlen(line));
    } while(n > 0);
    
    close(sockFD);
    
    return 0;
}