#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>

//TCP -> create a socket, define addr to connect, try to connect, if sucessful trasnmit data
int main(int argc, char* argv[]){
    
    //create a socket (ipv4)
    int network_socket = socket(AF_INET, SOCK_STREAM, 0);

    //specify an adress for the socket
    struct sockaddr_in server_adress;
    server_adress.sin_family = AF_INET; //Type of adress
    server_adress.sin_port = htons(9002); //9002 is a network port not used on our computer
    server_adress.sin_addr.s_addr = INADDR_ANY; //INADDR_ANY is alocal adress

    //connect to port and server
    if( connect(network_socket, (struct sockaddr *) &server_adress, sizeof(server_adress)) == -1){
        printf("Error conecting!\n");
        exit(1);
    }else{
        printf("Sucessfully connected to remote socket!\n");
    }

    //creating buffer to receive info
    char* buffer = (char*)malloc(1024*sizeof(char));
    if(buffer == NULL){
        printf("Error allocating memory!\n");
        exit(1);
    }
    //receiving data from remote socket
    if(recv(network_socket, buffer, 1024*sizeof(char), 0) == -1){
        printf("Error receiving data!\n");
        exit(1);
    }

    //printf info received
    printf("Server:\n%s\n", buffer);

    for(int i = 0; i < 1024; i++){
        buffer[i] = '\0';
    }
    strcpy(buffer, "Client: data received\n");
    if(send(network_socket, buffer, 1024*sizeof(char), 0) == -1){
        printf("Error sending data!\n");
        exit(1);
    }
    
    //close the socket
    free(buffer);
    close(network_socket);

    return 0;
}