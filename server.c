#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

//TCP -> create socket, define server addr, bind to socket with addr and port specified, listen to connections, accept connections, transmit data
int main(int argc, char* argv[]){

    char* buffersend = (char*)malloc(1024*sizeof(char));
    if(buffersend == NULL){
        printf("Error alocating memory!\n");
        exit(1);
    }
    
    char* bufferrecv = (char*)malloc(1024*sizeof(char));
    if(bufferrecv == NULL){
        printf("Error alocating memory!\n");
        exit(1);
    }

    char* readfile = (char*)malloc(128*sizeof(char));
    if(readfile == NULL){
        printf("Error alocating memory!\n");
        exit(1);
    }

    int server_socket;
    int client_socket;
    int descision = 0;
    /*
    for(int index = 1; index < argc; index++){
        strcat(buffer, argv[index]);
        strcat(buffer, " ");
    }
    */

    //open html file
    FILE* htmlresponse = fopen("response.html", "r");
    if(htmlresponse == NULL){
        exit(1);
    }
    //read html file into buffer
    while(fgets(readfile, 128, htmlresponse) != NULL){
        strcat(buffersend, readfile);
    };
    
    fclose(htmlresponse);

    FILE* secondaryresponse = fopen("secondary.html", "r");
    if(secondaryresponse == NULL){
        exit(1);
    }


    //creating server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    //define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET; //type of connection
    server_address.sin_port = htons(9002); //socket port to connect
    server_address.sin_addr.s_addr = INADDR_ANY; //server address

    //bind the sockt to our specified IP and port
    bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    char choice;

    while(1){

        printf("Accept another connection? [y/n]\n");
        scanf("%c",&choice);
        if(strcmp(&choice, "n") == 0){
            break;
        }
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

        //send the message
        if(send(client_socket, buffersend, 1024*sizeof(char), 0) == -1){
            printf("Error sending data!\n");
            exit(1);
        }
        
        //receive response
        if(recv(client_socket, bufferrecv, 1024*sizeof(char), 0) == -1){
            printf("Error receiving data!\n");
            exit(1);
        }

        printf("%s", bufferrecv);

        close(client_socket);
        shutdown(client_socket, SHUT_RDWR);
    }

    close(server_socket);
    //close the socket
    free(readfile);
    free(bufferrecv);
    free(buffersend);
    

    return 0;
}