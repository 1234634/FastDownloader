/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2019/2020
    Semestar:       Zimski (V)
    
    Ime fajla:      server.c
    Opis:           TCP/IP server
    
    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27018
#define MAX_FILES 20
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server ,server1,client;
    char client_message[DEFAULT_BUFLEN];
    char* available_files;
    available_files = "Need for speed ;Slender man ;Counterstrike 1.6 v42 ;HALF LIFE;";
   
    //Create socket, their protocol specifications
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure, adress of the socket prep
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEFAULT_PORT); //Bind socket that contains protocol info + socket_addr 
    
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen (socket,num.  of max connections)
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");
  




  /////////////////////////////////////////////////////////////////////////////
   

    int socket_sender = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket sender created ");





   // while( (read_size = recv(client_sock , client_message , DEFAULT_BUFLEN , 0)) > 0 )
     read_size = recv(client_sock , client_message , DEFAULT_BUFLEN , 0);
    //{
        printf("Bytes received: %d\n", read_size);
    //}
 
    printf("Message recieved: %s\n",client_message);
    
	
read_size = recv(client_sock , client_message , DEFAULT_BUFLEN , 0);

    printf("Message recieved: %s\n",client_message);

    

    //Prepare the sockaddr_in structure, adress of the socket prep
    server1.sin_family = AF_INET;
    server1.sin_addr.s_addr = client.sin_addr.s_addr;
    server1.sin_port = htons(27017); //Bind socket that contains protocol info + socket_addr 
    //Connect to remote server
    if (connect(socket_sender , (struct sockaddr *)&server1 , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
	printf("Passed the connect");

    if(send(client_sock, available_files, strlen(available_files),0) < 0) 
    {
    	puts("Send failed");
	return 1;	
    }
	
    printf("Sending done");
	


    //Receive a message from client
    

   
    
    
    
    
    /*// lets see what is asked to be downloaded
	switch(client_message[0])
	{
		case 't':
			printf("Downloading t\n");
			break;
		default:
			printf("We don't have that file");
			break;
	
	}*/

    while( (read_size = recv(client_sock , client_message , DEFAULT_BUFLEN , 0)) > 0 )
    {
        printf("Bytes received: %d\n", read_size);
    }
 
    printf("Message recieved: %s\n",client_message);

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    return 0;
}

