/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2019/2020
    Semestar:       Zimski (V)
    
    Ime fajla:      client.c
    Opis:           TCP/IP klijent
    
    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include<stdio.h>      //printf
#include<string.h>     //strlen
#include<sys/socket.h> //socket
#include<arpa/inet.h>  //inet_addr
#include <fcntl.h>     //for open
#include <unistd.h>    //for close

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27019
#define CLIENT_PORT 27017

int main(int argc , char *argv[])
{
    int c,sock,sock_desc,read_size,client,client_sock;
    struct sockaddr_in server;
    char *message = "this is a test";
    char available_files[DEFAULT_BUFLEN];

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    
    //Send some data
    if( send(sock , message , strlen(message), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    puts("Client message:");
    puts(message);


    //Receive a message from client
     (read_size = recv(sock , available_files , DEFAULT_BUFLEN , 0)); 
    {
        printf("Bytes received: %d\n", read_size);
    }
 
    printf("Message recieved: %s\n",available_files);
    puts("Connected\n");
    
    close(sock);
    //close(sock_desc);
    return 0;
}

