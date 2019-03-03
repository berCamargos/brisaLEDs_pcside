#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "udp_client.h"



static int sockfd;
static int serverlen;
static struct sockaddr_in serveraddr;

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
}

int connect_udp(char *hostname, int portno)
{
    struct hostent *server;

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        return -1;
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);
    return 0;
}

int send_udp(uint8_t *msg, uint16_t msg_len)
{

    int n;
    /* send the message to the server */
    serverlen = sizeof(serveraddr);
    n = sendto(sockfd, msg, msg_len, 0, (struct sockaddr *)&serveraddr, serverlen);
    if (n < 0) 
      printf("ERROR in sendto");
    return 0;
}


