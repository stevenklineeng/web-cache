/** Steven Kline ; CSCE 3530 programming assignment 2 ; 02/19/20
  * lab1svr.c :: the code for the proxy server side of this program ; recieves a
  * URL from the client, and performs the needed actions upon it
 **/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>
#include <netinet/in.h>

void GetTime();
void URL_to_IP(char * URL, char * IP);

char URLcache[5][2][300]; 
struct sockaddr_in servaddrCLI;
 
int main(int argc, char **argv) {
	/** DECLARES VARIABLES **/
	int svr_portno;
    char str[600000];
    int listen_fd, conn_fd, sockfd, n;
	int len = sizeof(struct sockaddr);
    struct sockaddr_in servaddr;
	char * client_message = malloc(256*sizeof(char));
	int nread, hostinfo;
	bool flag = 0;
	FILE * file;
	char temp[600000];
	char http_msg[400];
	char * ip_addy = calloc(30,sizeof(char));
	
	// makes sure that a correct number of cmd line args are provided
	if(argc != 2) { 
		printf("Incorrect number of command line arguments; exiting... \n"); 
		exit(1); 
	}
 
		/** SETS UP SERVER PORTION **/
    // AF_INET - IPv4 IP
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
 
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	svr_portno = atoi(argv[1]);
    servaddr.sin_port = htons(svr_portno);
 
    /* Binds the socket */
	bind(listen_fd,  (struct sockaddr *) &servaddr, sizeof(servaddr));

	/* Start listening to incoming connections */
	listen(listen_fd, 10);
		/** END OF SERVER PORTION **/

		/** BEGINS INTERACTION WITH CLIENT **/
	for(int i = 0; i < 5; i++) // empties out URLcache
		for(int x = 0; x < 2; x++)
			bzero(URLcache[i][x], 300);
		
	while(1) {
		/* Accepts an incoming connection */
		conn_fd = accept(listen_fd, (struct sockaddr*)NULL, NULL);
		bzero(str, 600000); bzero(client_message, 256);
		
		/* begins reading in message from client */
		while ((nread = recv(conn_fd, client_message, 256, 0)) > 0) {
			if(strcmp(client_message, "quit") == 0) { 
				exit(0); 
			} 

			/* checks cache for URL and sends if present */
			for(int i = 0; i < 5; i++) {
				if(strcmp(client_message, URLcache[i][0]) == 0) {
					// sets str to full webpage from file
					file = fopen(URLcache[i][1], "r");
					bzero(temp, 600000); bzero(str, 600000);
					if(fgets(temp, 600000, file) != NULL) { 
						strcpy(str, temp); 
						bzero(temp, 600000); 
					}
					while(fgets(temp, 600000, file) != NULL) { 
						strcat(str, temp); 
						bzero(temp, 600000); 
					}
					fclose(file);
					
					write(conn_fd, str, strlen(str)); // write to the client
					
					// shows that this was the case and breaks
					flag = 1; break;
				}
			}

			/* contacts URL server and reconfigures cache otherwise */
			if(flag == 0) {
					/** START OF CLIENT PORTION **/
				bzero(http_msg, 400);
				sprintf(http_msg, "GET / HTTP/1.1\r\nHost: %s\r\n\r\n", client_message);

				sockfd=socket(AF_INET, SOCK_STREAM, 0);
				
				bzero(&servaddrCLI,sizeof(servaddrCLI));

				URL_to_IP(client_message, ip_addy);			
				inet_pton(AF_INET,ip_addy,&(servaddrCLI.sin_addr)); // sets IP address ip_addy

				servaddrCLI.sin_family=AF_INET;
				servaddrCLI.sin_port=htons(80); // Server port number

				connect(sockfd,(struct sockaddr *)&servaddrCLI,sizeof(servaddrCLI));
				
				if(send(sockfd, http_msg, strlen(http_msg), 0) < 0) { 
					perror("send"); 
					exit(EXIT_FAILURE); 
				}

				usleep(100000);
				while ((n = read(sockfd, str, sizeof(str))) > 0) { 
					break; 
				}
				close(sockfd);
					/** END OF CLIENT PORTION **/
					
				
				if(str[9] == '2' && str[10] == '0' && str[11] == '0') { // puts in cache if code is 200
					if(URLcache[4][1][0] != '\0') { 
						remove(URLcache[4][1]); 
					}
					
					for(int i = 3; i >= 0; i--) { // moves items in cache back by one
						if(URLcache[i][0][0] != '\0') {
							bzero(URLcache[i+1][0],300); bzero(URLcache[i+1][1],300);
							strcpy(URLcache[i+1][0], URLcache[i][0]);
							strcpy(URLcache[i+1][1], URLcache[i][1]);
						}
					}

					// sets newest URL in cache
					bzero(URLcache[0][0], 300); bzero(URLcache[0][1], 300);
					strcpy(URLcache[0][0], client_message);
					GetTime();
							
					// updates list.txt
					file = fopen("list.txt", "w");
					for(int i = 0; i < 5; i++) { fprintf(file, "%s %s\n", URLcache[i][0], URLcache[i][1]); }
					fclose(file);

					// puts webpage in file
					file = fopen(URLcache[0][1], "w");
					fprintf(file, "%s", str);
					fclose(file);
				}
				write(conn_fd, str, strlen(str)); // write to the client
			}
			// clears out arrays/variables
			flag = 0;
			bzero(client_message, 256); bzero(str, 600000);
			fflush(stdin); fflush(stdout);
		}
		  close (conn_fd); //close the connection
    }
		/** END INTERACTION WITH CLIENT **/
}

/** FUNCTION THAT GETS CURRENT TIME AND STORES IN URLcache **/
void GetTime() {
	time_t currTime = time(NULL);
	int year, month, day, hour, minute, second;
	struct tm * dateTime = localtime(&currTime);

	bzero(URLcache[0][1], 300);
	
	year = dateTime->tm_year + 1900;
	month = dateTime->tm_mon + 1;
	day = dateTime->tm_mday;
	hour = dateTime->tm_hour;
	minute = dateTime->tm_min;
	second = dateTime->tm_sec;
	
	sprintf(URLcache[0][1], "%d%02d%02d%02d%02d%02d", year, month, day, hour, minute, second);
}

/** FUNCTION THAT FINDS THE IP FROM THE URL **/
void URL_to_IP(char * URL, char * IP) {
	char * temp = malloc(64*sizeof(char)); 
	bzero(temp, 64);
	
	struct hostent *url_to_ip;
	url_to_ip = gethostbyname(URL);
	
	    int i = 0;
        while(url_to_ip->h_addr_list[i] != NULL) {
            sprintf(IP, "%s",inet_ntoa( (struct in_addr) *((struct in_addr *) url_to_ip->h_addr_list[i])));
			i++;
        }
 }	