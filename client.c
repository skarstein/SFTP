/* 
 * client.c
 * Sean Karstein
 * COEN 146L
 * 1/18/18
 *
 * FTP client that sends a file to server.c with TCP
 *
 * Usage:
 * Once the server is listening for a connection:
 * 	$ ./client FILE_TO_TRANSFER NEW_FILENAME HOST PORT
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

#define BUFF_LEN 10             // num bytes to transfer per iteration

void checkResult(int, char*);
int ftpClient(char *filename, char* newFileName, char* host, char* port);


int main(int argc, char **argv){
    if (argc < 5){
        printf("Usage: %s FILE_TO_TRANSFER NEW_FILENAME HOST PORT\n", argv[0]);
        exit(-1);
     }

    if (access(argv[1], R_OK) != 0){
        printf("Make sure you have read access to the file\n");
        exit(-1);
    }

    if (atoi(argv[4]) < 5000){
        printf("Please make sure PORT_NO is above 5000\n");
        exit(-1);
    }

    ftpClient(argv[1], argv[2], argv[3], argv[4]);
}

/*
 * int ftpserver(char *port)
 *
 * Starts an ftp client connecting to the given host:port, transferring over the contents
 * of filename into newFileName.
 *
 * @filename{char*} - name of the file to be sent
 * @newFileName{char*} - name of the file to be written
 * @host{char*} - IP address of the host server
 * @port{char*} - port number of host
 */

int ftpClient(char *filename, char* newFileName, char* host, char* port){
    int sockfd = setupConnection(host, port);
    char buffer[BUFF_LEN];
    FILE *fp=fopen(filename,"r");
    int fildes=fileno(fp);
    int bytes_written,bytes_read;
    void *q;                            //pointer to interate through file

    bzero(buffer,BUFF_LEN);
    sprintf(buffer,"%d",(strlen(newFileName)));

    //send the length of the filename
    while(1){
        write(sockfd,buffer,BUFF_LEN);
        bytes_read=read(sockfd,buffer,1);
        if(buffer[0]=='a')
            break;
    }

    //send the filename
    while(1){
        write(sockfd,newFileName,strlen(newFileName));
        bytes_read=read(sockfd,buffer,1);
        if(buffer[0]=='b')
            break;
    }

    //start sending the actual file
    while(1){
        bytes_read = read(fildes,buffer,sizeof(buffer));
        if(bytes_read==0)
            break;
        q=buffer;
        while(bytes_read>0){
            bytes_written=write(sockfd,q,bytes_read);
            bytes_read-=bytes_written;
            q += bytes_written;
        }
    }
    
    fclose(fp);
    close(sockfd);
    return 0;
}


/*
 * void checkResult(int retval, char *message)
 *
 * Prints the given message to stdout if retval is -1
 *
 * @retval{int} - number were checking
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
 * int setupConnection(char* host, char* port);
 *
 * Initializes client socket using passed in host and port
 * then attempt to connect to server.
 *
 * @host{char*} - string containing the ip address of the host
 * @port{char*} - string containing the port number
 *
 * @return{int} - socket file descriptor
 *
 */
int setupConnection(char* host, char* port){
    int sock;
    int success;
    struct addrinfo *sockList;
    struct sockaddr_in servAddr;

    //setup client socket
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    fprintf(stderr, "Client socket is set up\n");

    //Resolve the IP Address given the hostname and the port number 
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family=AF_INET;
    servAddr.sin_addr.s_addr=inet_addr(host);
    servAddr.sin_port=htons(atoi(port));

    //Try to connect to the server.
    success=connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr));
    checkResult(success, "could not connect to server\n");
    fprintf(stderr, "Successfully connected to server.\n");

    return sock;
}
