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

struct addrinfo *result = NULL, *ptr = NULL, hints;

typedef struct forAcceptingConnection{
    SOCKET TEMP;
    unsigned status;
}res;


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

int handleConnection(SOCKET tmpsocket){
    SOCKET tempSocket = INVALID_SOCKET;
    res clientSocket;
    clientSocket.TEMP = tempSocket;
    //sent to -1 for now
    clientSocket.status = -1;

    tempSocket = accept(tmpsocket, NULL, NULL);

    if(tempSocket == INVALID_SOCKET){
        return clientSocket.status;
    }
    
    clientSocket.status = 0;
    return clientSocket.status;
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


    //lets accept connection
    if(handleConnection < 0 ){
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
    else{
        fprintf(stdout,"Successfully accepting connections\n");
    }


//create a function to handle cleanup later
    freeaddrinfo(result);
    result = NULL;
    closesocket(server);
    //clean up 
    WSACleanup();

}