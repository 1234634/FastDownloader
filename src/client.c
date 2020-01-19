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
#include<stdlib.h>
#include<sys/socket.h> //socket
#include<arpa/inet.h>  //inet_addr
#include <fcntl.h>     //for open
#include <unistd.h>    //for close
#include<pthread.h> 

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27018
#define CLIENT_PORT 27017
#define DEFAULT_THREADSNUM 3 

typedef struct Packets
{
	int socket;
	char message[DEFAULT_BUFLEN];
	int length;
}Packet;

void *recievingThread(void *arg_packet)
{
	Packet* recievedData = arg_packet;
	char confirmMessage[DEFAULT_BUFLEN];
	strcpy(confirmMessage,"Confirming the transaction");

	//	pthread_mutex_lock(&lock);
//recievedData->length 
		
	if(recv(recievedData->socket , recievedData->message ,recievedData->length, 0) <= 0)
	{
		puts("Recieve failed");
		exit(0);
	}
	printf("message recieved %s \n",recievedData->message);
	
	if( send( recievedData->socket , confirmMessage , strlen(confirmMessage ) ,0 ) < 0 ) 
    		{
    			puts("Send failed");
			exit(0);	
   	 	}
	
	//pthread_mutex_unlock(&lock);

	return NULL;
}


int main(int argc , char *argv[])
{
    int sock[DEFAULT_THREADSNUM],read_size,i;
    struct sockaddr_in server;
    char *message = "this is a test";
    char *mes = "1";
    char available_files[DEFAULT_BUFLEN];
    Packet threadPacket[DEFAULT_THREADSNUM];
    void *argPacket;

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);

    //Connect to remote server
    for( i =0; i <DEFAULT_THREADSNUM; i++)
    {

    	//Create socket
    	sock[i] = socket(AF_INET , SOCK_STREAM , 0);
    	if (sock[i] == -1)
    	{
        	printf("Could not create socket");
    	}
    	puts("Socket created");
    	
	if (connect(sock[i] , (struct sockaddr *)&server , sizeof(server)) < 0)
    	{
        	perror("connect failed. Error");
        	return 1;
    	}
    }
    
    //Send some data
    if( send(sock[0] , message, strlen(message), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }
   			
     (read_size = recv(sock[0] , available_files , DEFAULT_BUFLEN , 0)); 
    {
        printf("Bytes received: %d\n", read_size);
    }
 
    int threadsNumber = atoi(available_files);
    printf("Message recieved: %s\n",available_files);
   

    memset(available_files,0,sizeof(available_files));
    //Receive a message from client
     (read_size = recv(sock[0] , available_files , DEFAULT_BUFLEN , 0)); 
     if(read_size <= 0)
	{
		puts("Recieve failed");
		return 1;
	}
    {
        printf("Bytes received: %d\n", read_size);
    }
    printf("Message recieved: %s\n",available_files);
   //Send some data
    if( send(sock[0] , mes , strlen(mes), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }
    
    memset(available_files,0,sizeof(available_files));
    (read_size = recv(sock[0] , available_files , DEFAULT_BUFLEN , 0)); 
     if(read_size <= 0)
	{
		puts("Recieve failed");
		return 1;
	}
    {
        printf("Bytes received: %d\n", read_size);
    }
    printf("Message recieved: %s\n",available_files);
    
   int size = atoi(available_files); 
   printf("SIZE IS %d\n",size); 
    

    
	pthread_t *tid =(pthread_t*) malloc(threadsNumber * sizeof(pthread_t));

    for(i=0; i< threadsNumber; i++)
    {
    
		    
		threadPacket[i].socket = sock[i];
		if( i != (threadsNumber -1))
		     threadPacket[i].length = size/DEFAULT_THREADSNUM;
		else
		     threadPacket[i].length = size - (size / DEFAULT_THREADSNUM * (threadsNumber - 1)) + 2;
		
		printf("LEngt %d of %d",threadPacket[i].length,i);
		argPacket= &threadPacket[i];

		pthread_create(&(tid[i]),NULL,&recievingThread,argPacket); 
	       
    }    
    for(i= 0; i < threadsNumber; i++)
    {
	pthread_join(tid[i],NULL); 
    } 

    for(i=0; i< DEFAULT_THREADSNUM; i++)
    	close(sock[i]);
    
    return 0;
}

