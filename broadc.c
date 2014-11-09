#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include<sys/time.h>       

#define BUFSIZE 1024
#define CLOCKS_PER_SEC 1000

clock_t t,t2;

void send_recv(int i, int sockfd)
{
        char send_buf[BUFSIZE];
        char recv_buf[BUFSIZE];
        int nbyte_recvd;
        
        if (i == 0){
                fgets(send_buf, BUFSIZE, stdin);

                if (strcmp(send_buf , "quit\n") == 0) {
                        exit(0);
                }
		else {
                        send(sockfd, send_buf, strlen(send_buf), 0);
			t=clock();
		}
        }
	else {
		t2=clock()-t;

		if(t2/CLOCKS_PER_SEC > 60)
		exit(0);

                nbyte_recvd = recv(sockfd, recv_buf, BUFSIZE, 0);
                recv_buf[nbyte_recvd] = '\0';
                printf("%s\n" , recv_buf);
                fflush(stdout);
        }
}

void connect_request(int *sockfd, struct sockaddr_in *server_addr)
{
	int port,c=0,flag=1;
	char ip[15];

        if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
                perror("Socket");
                exit(1);
        }

	do{
		fflush(stdin);
		printf("Enter IP address of the server : ");
		scanf("%s",ip);
		fflush(stdin);
		printf("Enter port no.");
		scanf("%d",&port);
		fflush(stdin);
	        server_addr->sin_family = AF_INET;
	        server_addr->sin_port = htons(port);
	        server_addr->sin_addr.s_addr = inet_addr(ip);

	        memset(server_addr->sin_zero, '\0', sizeof server_addr->sin_zero);

	        if(connect(*sockfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1) {
	                perror("connect");
	                c++;
        }
	else
		flag=0;
	}while(c<3 && flag);

	if(c==3)
	{
		printf("Request max. limit is reached...Try again\n");
		exit(1);
	}
}


int main()
{
        int sockfd, fdmax, i, n, c=0;
        struct sockaddr_in server_addr;
        fd_set master;
        fd_set read_fds;

	char send_buf[BUFSIZE],recv_buf[BUFSIZE];
	struct timeval timeout;

        connect_request(&sockfd, &server_addr);
        FD_ZERO(&master);
        FD_ZERO(&read_fds);
        FD_SET(0, &master);
        FD_SET(sockfd, &master);
        fdmax = sockfd;

        puts("Enter the Username you want : ");

        while(1){
		timeout.tv_sec=60;
		timeout.tv_usec=0;
                read_fds = master;
                if(select(fdmax+1, &read_fds, NULL, NULL, &timeout) == 0){
                        printf("timeout occured...Client quitted\n");
			send(sockfd,"quit\n",5,0);
                        close(sockfd);
			return 0;
                }
		
		if(c==0)
		{
			
			fgets(send_buf,BUFSIZE,stdin);
			send(sockfd, send_buf, strlen(send_buf), 0);
			c++;
		}
		
                for(i=0; i <= fdmax; i++ )
                        if(FD_ISSET(i, &read_fds))
                                send_recv(i, sockfd);
        }
        printf("client-quited\n");
        close(sockfd);
        return 0;
}
