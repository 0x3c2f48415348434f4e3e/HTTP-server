#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>

//#pragma comment(lib, "Ws2_32.lib")

//our default port
#define PORT "5000"
#define DEFAULT_BUFLEN 512

struct addrinfo *result = NULL, *ptr = NULL, hints;

typedef struct forAcceptingConnection{
    SOCKET TEMP;
    unsigned status;
}res;

char recvbuf[DEFAULT_BUFLEN];
int iResult, iSendResult;
int recvbuflen = DEFAULT_BUFLEN;
res forClient;

int initiliases(void){
    WSADATA wsaData;
    return  WSAStartup(MAKEWORD(2,2),&wsaData);
}

int setUpServer(void){
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    return getaddrinfo(NULL, PORT, &hints, &result); //to resolve the local address and port to be used by the server
}

SOCKET createSocketObject(void){ //for server
    SOCKET returnServerObject = INVALID_SOCKET;
    return returnServerObject;
}

int bindSocket(SOCKET tmpsocket){
    return bind(tmpsocket, result->ai_addr, (int) result->ai_addrlen); //casting from size_t to an int
}

int listeningSocket(SOCKET tmpsocket){
    return listen(tmpsocket, SOMAXCONN);
}

res handleConnection(SOCKET tmpsocket){
    SOCKET tempSocket = INVALID_SOCKET;
    res clientSocket;
    clientSocket.TEMP = tempSocket;
    //sent to -1 for now
    clientSocket.status = -1;

    tempSocket = accept(tmpsocket, NULL, NULL);

    if(tempSocket == INVALID_SOCKET){
        return clientSocket;
    }
    
    clientSocket.status = 0;
    return clientSocket;
}

int disconnectServer(res temp){ //temp must be the client
    iResult = shutdown(temp.TEMP,SD_SEND);
    if(iResult == SOCKET_ERROR){
        fprintf(stderr,"Shutdown failed: %d\n",WSAGetLastError());
        return -1;
    }
    return 0;
}

int main(int argc, char* argv[]){
    if(initiliases() != 0){
        fprintf(stderr,"Could not initiate\n");
        //no allocated memory so no need to deallocate anything
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else{
        fprintf(stdout,"Successfully initiated\n");

    }

    //server
    int res = setUpServer();
    if(res != 0){
        fprintf(stderr, "getaddrinfo failed: %d\n", res);
        //no allocated memeory so no need to deallocate anything
        WSACleanup();
        exit(EXIT_FAILURE);
        
    }
    else{
        fprintf(stdout, "getaddinfo successful: %d\n", res);
    }

    SOCKET server = createSocketObject();
    server = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if(server == INVALID_SOCKET){
        fprintf(stderr,"Error creating server socket: %i\n", WSAGetLastError());
        //need to deallocate memory
        freeaddrinfo(result);
        result = NULL;
        //clean up 
        WSACleanup();
    }
    else{
        fprintf(stdout,"Successfully create server\n");
    }

    //bind our socket
    int bindStatus = bindSocket(server);
    if(bindStatus == SOCKET_ERROR){
        fprintf(stderr,"Failed to bind socket: %d\n",WSAGetLastError());
        //will need to clean up before exiting
        //need to deallocate memory
        freeaddrinfo(result);
        result = NULL;
        //clean up 
        //also close our socket
        closesocket(server);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else{
        fprintf(stdout,"Successfully bind socket\n");
    }

    //check for any connections attempts
    //listening on our socket
    if(listeningSocket(server) == SOCKET_ERROR){
        fprintf(stderr,"Failed to bind socket: %d\n",WSAGetLastError());
        //will need to clean up before exiting
        //need to deallocate memory
        freeaddrinfo(result);
        result = NULL;
        //clean up 
        //also close our socket
        closesocket(server);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else{
        fprintf(stdout,"Successfully listening...\n");
    }

    while(1){
        //here is where we need to check for incoming connections

        //lets accept connection
        forClient = handleConnection(server);
        if(forClient.status < 0 ){
            fprintf(stderr,"failed to accept connection: %d\n",WSAGetLastError());
            //will need to clean up before exiting
            //need to deallocate memory
            freeaddrinfo(result);
            result = NULL;
            //clean up 
            //also close our socket
            closesocket(server);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        //lets receive some data

        //use a do...while loop
        do{
            iResult = recv(forClient.TEMP,recvbuf,recvbuflen,0);//need to pass in our temp client
            if(iResult > 0){
                printf("Bytes Received: %d\n",iResult);
            }
        
        }
        while(iResult > 0);
    }

    //since we hvae an infinite loop above, we need a way to be able to suitable disconnect the server
    if(disconnectServer(forClient) == -1){
        freeaddrinfo(result);
        result = NULL;
        closesocket(server);
        //clean up 
        WSACleanup();
    }


//create a function to handle cleanup later
    freeaddrinfo(result);
    result = NULL;
    closesocket(server);
    //clean up 
    WSACleanup();

}