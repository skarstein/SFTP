/*
 * server.c
 * Sean Karstein
 * COEN 146L
 * 1/18/18
 *
 * FTP server that receives a file from client.c with TCP
 *
 * Usage:
 *	$ ./server PORT_NO
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#define BUFF_LEN 10              // num bytes to transfer per iteration

void checkResult(int, char*);
void ftpserver(char *);

/*
 * int main(int argc, char **argv);
 *
 * @argc{int} - number of arguments
 * @argv{char**} - array of arguments
 */
int main(int argc, char **argv){

    //check proper number of arguments
    if (argc < 2){
        printf("Usage: ./%s PORT_NO\n", argv[0]);
        exit(-1);
    }

    //check port number is greater 1024
    if (atoi(argv[1]) < 1024){
        printf("PORT_NO must be greater than 1024\n");
        exit(-1);
    }

    ftpserver(argv[1]);
 }


/*
 * void ftpserver(char *port)
 *
 * Starts an FTP server on the given port and waits to
 * receive and write to the given file.
 *
 * @port{char*} - string containing the port number
 */

void ftpserver(char *port){
    int i,readsize;
    int sockfd = setupSingleConnection(port);
    char buffer[BUFF_LEN];

    FILE *fp;
    char fname[100];
    void *q=fname;
    printf("receiving file...\n");
    int bytesReceived;
    int nameSize;

    //receive the length of the filename
    while(1){
        bytesReceived = read(sockfd, buffer, BUFF_LEN);
        if(bytesReceived>0){
            nameSize=atoi(buffer);
            printf("the size of the file is: %d\n",nameSize);
            break;
        }
    }

    write(sockfd,"a",1);

    //receive the filename    
    if(bytesReceived = read(sockfd, fname, nameSize)>0){
        write(sockfd,"b",1);
    }

    fname[nameSize]='\0';
    printf("the filename should be: %s\n",fname);
    fp=fopen(fname,"ab");

    //receive the file
    while((bytesReceived = read(sockfd, buffer, BUFF_LEN))>0){
        fwrite(buffer,1,bytesReceived,fp);
    }
    fclose(fp);
    close(sockfd);
}

/*
 * void checkResult(int retval, char *message)
 *
 * Prints the given message to stdout if retval is -1
 *
 * @retval{int} - number the function is checking
 * @message{char*} - error message
 *
 */
void checkResult(int retval, char *message){
    if (retval == -1){
        printf("%s.\nError Code: %d", message, errno);
        exit(-1);
    }
}

/* 
 * int setupSingleConnection(char* port);
 *
 * Set up a TCP server socket with the passed in port
 * number and wait for a connection.
 * Once a connection is established, return the socket
 * file descriptor.
 *
 * @port{char*} - string containing the port number
 *
 * @return{int} - socket file descriptor
 */
int setupSingleConnection(char* port){
    int success;
    int listeningSocket;
    int communicatingSocket;

    struct addrinfo hints;
    struct addrinfo *sockList;
    struct sockaddr_storage client_addr;

    //set hints to 0
    memset(&hints, 0, sizeof(hints));

    //family flag set to address family
    hints.ai_family = AF_INET;
    //socktype flag set to TCP
    hints.ai_socktype = SOCK_STREAM;
    //flags must be AI_PASSIVE to make call to accept(), man getaddrinfo
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

    //get socket file descriptor
    getaddrinfo(NULL, port, &hints, &sockList);
    fprintf(stderr, "Got address info\n");

    //bind socket to local host and port
    listeningSocket = socket(sockList->ai_family, sockList->ai_socktype, sockList->ai_protocol);
    fprintf(stderr, "Listening socket is set up\n");
    //pass in whatever socket specs we had as well as the address we want to send to
    success = bind(listeningSocket, sockList->ai_addr, sockList->ai_addrlen);
    checkResult(success, "could not bind to socket\n");
    fprintf(stderr, "Successfully bound to listening socket.\n");

    //bound, ready to start listening for  connections
    //1 represents number of things on backlog (were only listening for 1 thing
    //before we close socket)
    success = listen(listeningSocket, 1);
    checkResult(success, "Could not listen on the specified interface.\n");
    fprintf(stderr, "Listening for connections\n");

    //poll for all the data and process it
    //accept is a blocking call, but listen is not, it'll return immediately
    socklen_t address_size=sizeof(client_addr);
    communicatingSocket = accept(listeningSocket, (struct sockaddr *)&client_addr, &address_size);
    fprintf(stderr, "Connection accepted, ready to start recieving data.\n");

    return communicatingSocket;
}



