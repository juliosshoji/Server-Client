#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#define MESSAGE_SIZE 4096

const char* breakupcode = "break connection please\n";

int breakupsignal = 1;

int openSocket();
void send_data(void* data, size_t content_size, int network_socket);
void* treat_data(int network_socket);
char* getMessage();
char* getResponse();
void printChat();

void* receiving(void* network_socket);
void* sending(void* network_socket);

//TCP -> create a socket, define addr to connect, try to connect, if sucessful trasnmit data
int main(int argc, char* argv[]){
    int socket = openSocket();
    
    pthread_t receiving_thread;
    pthread_t sending_thread;

    pthread_create(&receiving_thread, NULL, receiving, (void*) &socket);
    pthread_create(&sending_thread, NULL, sending, (void*) &socket);

    pthread_join(receiving_thread, NULL);
    pthread_join(sending_thread, NULL);

    close(socket);

    printf("Conexão terminada, fechando!\n");


    return 0;
}

void* receiving(void* network_socket){
    int* socket = (int*) network_socket;
    char* response;
    while(breakupsignal){
        response = treat_data(*socket);
        if(strncmp(response, breakupcode, 25) == 0){
            breakupsignal = 0;
            break;
        }
        printf("Servidor: %s", response);
    }
    return NULL;
}

void* sending(void* network_socket){
    int* socket = (int*) network_socket;
    char* message;
    while(breakupsignal){
        message = getMessage();
        send_data(message, strlen(message), *socket);
        if(strncmp(message, breakupcode, 25) == 0){
            breakupsignal = 0;
        }
    }
    return NULL;
}

void send_data(void* data, size_t content_size, int network_socket){
    

    if(send(network_socket, &content_size, sizeof(size_t), 0) == -1){
        printf("Error sending data in send_data 1!\n");
        exit(1);
    }

    if(send(network_socket, data, content_size, 0) == -1){
        printf("Error sending  data in send_data 2!\n");
        exit(1);
    }

}

void* treat_data(int network_socket){
    
    size_t content_size;

    //receiving data from remote socket
    if(recv(network_socket, &content_size, sizeof(size_t), 0) == -1){
        printf("Error receiving data in treat_data 1!\n");
        exit(1);
    }
    void* content = (void*)malloc(content_size);
    if(content == NULL){
        printf("Error alocating memory for content\n");
        exit(1);
    }

    if(recv(network_socket, content, content_size*sizeof(char), 0) == -1){
        printf("Error receiving data in treat_data 2!\n");
        exit(1);
    }

    return content;
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

int openSocket(){
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
    return network_socket;
}