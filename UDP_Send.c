
#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <errno.h>
#include <stdint.h>

#define BUFLEN 512  //Max length of buffer
#define PORT_RECEIVE 10001   //The port on which to listen for incoming data
#define PORT_SEND 10000

int main(int argc, char *argv[]) {
	
	struct sockaddr_in si_receive;
    struct sockaddr_in si_send;

	int s = sizeof(si_send);
    //int r = sizeof(si_receive);
    socklen_t addrlen = sizeof(si_send);     /* length of addresses */

    int recvlen;
    char buf[BUFLEN];
    char message[BUFLEN];
    char* walker; 
    char* server = "192.168.1.141";
    //char *server = "127.0.0.1"; /* change this to use a different server */

	//create a socket and check for error
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		printf("Socket was not created, quitting program");
		exit(0);
	}

	memset((char *) &si_send, 0, sizeof(si_send));
    si_send.sin_family = AF_INET;
    si_send.sin_port = htons(PORT_SEND);
    si_send.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(s, (struct sockaddr*) &si_send, sizeof(si_send)) == -1){
    	printf("Unable to bind socket to receive, quitting program");
    	exit(1);
    }
  

    memset((char *) &si_receive, 0, sizeof(si_receive));
    si_receive.sin_family = AF_INET;
    si_receive.sin_port = htons(PORT_RECEIVE);
    //si_receive.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (inet_aton(server, &si_receive.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    //printf("%x, %x, %x, %x\n", (uint16_t*)&si_receive.sin_port, (uint16_t*)&si_send.sin_port, (uint32_t*)&si_receive.sin_addr, (uint32_t*)&si_send.sin_addr);
    //walker = (char*)&si_send;
    //printf("%-02x,%-02x,%-02x,%-02x,%-02x,%-02x,%-02x,%-02x\n", walker[0], walker[1], walker[2], walker[3], walker[4] ,walker[5], walker[6], walker[7]);

    while(1) {

    	printf("Send a message\n");
    	gets(message);
        printf("Sending data...");

    	if (sendto(s, message, BUFLEN, 0,  (struct sockaddr*) &si_receive, addrlen) == -1) {
    		printf("Unable to send the message, quitting program");
    		printf("\n%s\n", strerror(errno));
            exit(1);
        }

        printf("Waiting for data...");
     	
     	if ((recvlen = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*) &si_receive, &addrlen)) == -1) {
     		printf("Unable to receive from socket, quitting program");
     		exit(1);	
    	}
    	else{
            buf[recvlen] = 0;
            printf("received message: \"%s\"\n", buf);
        }
    }
 
    return 0;
}