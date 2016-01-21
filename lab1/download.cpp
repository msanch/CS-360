
//
// Created by Martin on 1/18/2016.
//

#include "CS360Utils.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define BUFFER_SIZE 10000
#define SOCKET_ERROR -1
#define HOST_NAME_SIZE 255
#define GET_MAX 1000


int main(int argc, char *argv[]){

    int hSocket, hServerSocket;
    struct sockaddr_in Address;
    struct hostent* pHostInfo;
    int nAddressSize= sizeof(struct sockaddr_in);
    int nHostPort;
    char pBuffer[BUFFER_SIZE];
    char strHostName[HOST_NAME_SIZE];
    char *path;
    unsigned nReadAmount;
    long nHostAddress;
    int count, headers = 0;
    int holder;
    int param;
    // bound checking
    if (argc < 4){

        printf("\nUSAGE: download host-name host-port path [-c | -d]\n");
        return 0;
    }else{
        while( (param = getopt(argc, argv, "c:d")) != -1) {
            switch(param) {
                case 'c':
                    count = atoi(optarg);
                    break;
                case 'd':
                    headers = 1;
                    break;
                default:
                    abort();
            }
        }

        printf("headers = %d, count = %d\n", headers, count);
        strcpy(strHostName, argv[optind++]);
        nHostPort = atoi(argv[optind++]);
        path = argv[optind];

    }
    printf("\nMaking a socket\n");
    hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // this makes the document
    if(hSocket == SOCKET_ERROR){
        printf("\nFailed to make a socket\n");
        return 0;
    }

    
    pHostInfo = gethostbyname(strHostName); // get the IP address by name
    //printf("here\n");
     if(pHostInfo == NULL){
        printf("Unable to resolve dns name\n");
        return 0;
    }
    memcpy(&nHostAddress, pHostInfo->h_addr, pHostInfo->h_length);
	
    // fill the Address struct defined in the beginning
    Address.sin_addr.s_addr = nHostAddress;
    Address.sin_port = htons(nHostPort);
    Address.sin_family = AF_INET;
    //printf("here\n");
    // try connecting to the host

    printf("\nConnecting to %s (%X) on port %d\n\n", strHostName, nHostAddress, nHostPort);
    if(connect(hSocket, (struct sockaddr*) &Address, sizeof(Address)) == SOCKET_ERROR){
        printf("\nUnable to connect to host\n");
        return 0;
    }

    // Create the HTTP message
    char* message = (char*) malloc(GET_MAX);

    sprintf(message, "GET %s HTTP/1.1\r\nHost:%s:%d\r\n\r\n", path, strHostName, nHostPort);
    
    if(headers)
        printf("Request: %s\n", message);

    write(hSocket, message, strlen(message));

    // once I send the request read the response.
   // nReadAmount = read(hSocket, pBuffer, BUFFER_SIZE);
   

    vector<char*> header;
    char contentType[MAX_MSG_SZ];
    // read the status line

    char* startline = GetLine(hSocket);
    if(headers)
        printf("Status line: %s\n\n", startline);

    // read the header line
    //printf("Headers %d\n", headers);
    GetHeaderLines(header, hSocket, false);
    if(headers){
        //cout << "\nHere";
        for(int i =0; i < header.size(); i++){
            printf("%s\n", header[i]);

        }
    }

    if(count > 0){
        printf("Downloaded\n");
        for(int i=1; i < count; i++){
            //write(1, pBuffer, (unsigned)holder);
            write(hSocket, message, strlen(message));
            printf("Downloaded %d times\n", i+1);
        }
    }
    if(!count){
         while( (holder = read(hSocket, pBuffer, MAX_MSG_SZ)) > 0){
        write(1, pBuffer, (unsigned)holder);
        }

    }
   
    printf("\nClosing socket NOW\n");
    if(close(hSocket) == SOCKET_ERROR){
        printf("\nUnable to close socket\n");
        return 0;
    }


}
