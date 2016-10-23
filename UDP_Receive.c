
#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <errno.h>

#define BUFLEN 512  //Max length of buffer
#define PORT_RECEIVE 10001   //The port on which to listen for incoming data
#define PORT_SEND 10000


int main(int argc, char *argv[]) {

	struct sockaddr_in si_receive;  //My address
  struct sockaddr_in si_send; //Remote address

  socklen_t addrlen = sizeof(si_receive); //length of the address

  int r = sizeof(si_receive);  //My socket
  
  int recvlen; //Bytes received
  char buf[BUFLEN];  //Buffer length

  //Create my UDP socket
	if ((r = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		printf("Socket was not created, quitting program");
		exit(0);
	}

  memset((char *) &si_receive, 0, sizeof(si_receive));
  si_receive.sin_family = AF_INET;
  si_receive.sin_addr.s_addr = htonl(INADDR_ANY);
  si_receive.sin_port = htons(PORT_RECEIVE);

  if (bind(r, (struct sockaddr *)&si_receive, sizeof(si_receive)) < 0) {
    perror("Unable to bind");
    return 0;
  }

  while(1) {
    printf("waiting on port %d\n", PORT_SEND);

    recvlen = recvfrom(r, buf, BUFLEN, 0, (struct sockaddr*) &si_send, &addrlen);
    if (recvlen == -1) {
    	printf("Unable to receive from socket, quitting\n");
     	exit(1);	
    }

    else {
      buf[recvlen] = 0;
      printf("Message from %s:%d\n", inet_ntoa(si_receive.sin_addr), ntohs(si_receive.sin_port));
      printf("Received Message: %s\n", buf);
    }

    if (sendto(r, buf, strlen(buf), 0, (struct sockaddr *)&si_send, addrlen) < 0)
      perror("sendto");
  }
}



