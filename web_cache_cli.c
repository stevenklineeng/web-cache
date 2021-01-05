/** Steven Kline
  * lab1cli.c :: the code for the client side of this program ; reads in the user-input
  *              sentence, sends it to the server, and then recieves (and prints) the 
  *              information provided from the server.
 **/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
 
int main (int argc, char **argv) {
	int cli_portno, sockfd, n;
    int len = sizeof(struct sockaddr);
    char recvline[600000];
    struct sockaddr_in servaddr;
	char * usr_msg = malloc(256*sizeof(char));
	
	// makes sure that a correct number of cmd line args are provided
	if(argc != 2) { 
		printf("Incorrect number of command line arguments; exiting... \n"); 
		exit(1); 
	}

    /* AF_INET - IPv4 IP*/
    sockfd=socket(AF_INET, SOCK_STREAM, 0);
	
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
	cli_portno = atoi(argv[1]);
    servaddr.sin_port=htons(cli_portno);
 
    /* Convert IP addresses */
    inet_pton(AF_INET,"129.120.151.94",&(servaddr.sin_addr));
 
    /* Connect to the server */
    connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

	// starts indefinitely reading in lines from user and sending to server
	while ( 1 ) {
		// gets input from user
		printf("\nurl: ");
		scanf(" %[^\n]", usr_msg);
		
		// continues if incorrect website
		if(usr_msg[0] != 'w' || usr_msg[1] != 'w' || usr_msg[2] != 'w') { 
			fflush(stdin); fflush(stdout);
			bzero(recvline,600000); 
			bzero(usr_msg, 256);
			continue; 
		}
		
		// sends user input to server
		if(send(sockfd, usr_msg, strlen(usr_msg), 0) < 0) { 
			perror("send"); 
			exit(EXIT_FAILURE); 
		}
	
		// closes if "quit"
		if(strcmp(usr_msg, "quit") == 0) { 
			usleep(500); 
			exit(0); 
		}
		
		// reads in servers response
		usleep(500000);
		while ((n = read(sockfd, recvline, sizeof(recvline))) > 0) {
			printf("%s",recvline); // print the received text from server
			break;
		}
     	fflush(stdin); fflush(stdout);
	 	bzero(recvline,600000); bzero(usr_msg, 256);
	}
}
