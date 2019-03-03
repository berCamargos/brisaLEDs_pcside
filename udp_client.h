#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

int connect_udp(char *hostname, int portno);
int send_udp(uint8_t *msg, uint16_t msg_len);

#endif
