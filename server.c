#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

#define MESSAGE_SIZE 4096

const char* breakupcode = "break connection please\n";

int breakupsignal = 1;

int* openSocket();
void* treat_data();
void send_data(void* data, size_t content_size, int client_socket);
char* getMessage();

void* receiving(void* socket);
void* sending(void* socket);


//TCP -> create socket, define server addr, bind to socket with addr and port specified, listen to connections, accept connections, transmit data
int main(int argc, char* argv[]){
    int* sockets = openSocket();
    int client_socket = sockets[0];
    int server_socket = sockets[1];

    pthread_t receiving_thread;
    pthread_t sending_thread;

    pthread_create(&receiving_thread, NULL, receiving, (void*) &client_socket);
    pthread_create(&sending_thread, NULL, sending, (void*) &client_socket);

    pthread_join(receiving_thread, NULL);
    pthread_join(sending_thread, NULL);

    close(client_socket);
    close(server_socket);

    printf("Conexão terminada, fechando!\n");

    free(sockets);
    return 0;
}

void* receiving(void* socket){
    int* client_socket = (int*) socket;
    char* response;
    while(breakupsignal){
        response = treat_data(*client_socket);
        if(strncmp(response, breakupcode, 25) == 0){
            breakupsignal = 0;
            break;
        }
        printf("Cliente: %s", response);
    }
    return NULL;
}

void* sending(void* socket){
    int* client_socket = (int*) socket;
    char* message;
    while(breakupsignal){
        message = getMessage();
        send_data(message, strlen(message), *client_socket);
        if(strncmp(message, breakupcode, 25) == 0){
            breakupsignal = 0;
        }
    }
    return NULL;
}

void* treat_data(int client_socket){
    
    size_t content_size;
     //receive response
    if(recv(client_socket, &content_size, sizeof(size_t), 0) == -1){
        printf("Error receiving data in treat_data 1!\n");
        exit(1);
    }

    void* bufferrecv = (void*)malloc(content_size);
    if(bufferrecv == NULL){
        printf("Error alocating memory for bufferrecv\n");
        exit(1);
    }

    if(recv(client_socket, bufferrecv, content_size, 0) == -1){
        printf("Error receiving data in treat_data 2!\n");
        exit(1);
    }

    return bufferrecv;
}

void send_data(void* data, size_t content_size, int client_socket){

    //send the message
    if(send(client_socket, &content_size, sizeof(size_t), 0) == -1){
        printf("Error sending data in send_data 1!\n");
        exit(1);
    }

    if(send(client_socket, data, content_size, 0) == -1){
        printf("Error sending data in send_data 1!\n");
        exit(1);
    }

}

char* getMessage(){
    
    char* message = (char*)malloc(sizeof(char)*MESSAGE_SIZE);
    if(message == NULL){
        printf("Error alocating memory\n");
        exit(1);
    }
    fgets(message, MESSAGE_SIZE, stdin);

    return message;
}

int* openSocket(){
    int server_socket;
    int client_socket;    

    //creating server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    //define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET; //type of connection
    server_address.sin_port = htons(9002); //socket port to connect
    server_address.sin_addr.s_addr = INADDR_ANY; //server address

    //bind the sockt to our specified IP and port
    bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    //listen function to start listen to connections
    if(listen(server_socket, 10) == -1){
        printf("Error listening to connections!\n");
        exit(1);
    }  

    //accepting connection
    client_socket = accept(server_socket, NULL, NULL);
    if(client_socket == -1){
        printf("Error accepting client!\n");
        exit(1);
    }

    int* sockets = (int*)malloc(sizeof(int)*2);
    if(sockets == NULL){
        printf("Error alocaitng for sockets\n");
        exit(1);
    }
    sockets[0] = client_socket;
    sockets[1] = server_socket;

    return sockets;
}