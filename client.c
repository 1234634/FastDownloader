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
#include<time.h>

#define DEFAULT_BUFLEN 20000
#define DEFAULT_PORT   27019
#define CLIENT_PORT 27017
#define DEFAULT_THREADSNUM 10
#define DEFAULT_FILE_NUM "1" 

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

		
	if(recv(recievedData->socket , recievedData->message ,recievedData->length, 0) <= 0)
	{
		puts("Recieve failed");
		exit(0);
	}
	
	
	if( send( recievedData->socket , confirmMessage , strlen(confirmMessage ) ,0 ) < 0 ) 
    		{
    			puts("Send failed");
			exit(0);	
   	 	}
	
	return NULL;
}


int main(int argc , char *argv[])
{
    int threadsNumber = DEFAULT_THREADSNUM;
    char *mes = "1";
    
  
    if( argc == 2)
    {
        threadsNumber = atoi(argv[1]);
    }
    else if (argc >= 3)
    {
        threadsNumber = atoi(argv[1]);
	mes = argv[2];    	
    }

    int *sock,read_size,i;
    struct sockaddr_in server;
    char *message = "starting message";
    char available_files[DEFAULT_BUFLEN], serverMessage[DEFAULT_BUFLEN];
    Packet* threadPacket;
    FILE* fp;
    void *argPacket;
    sock = (int *) malloc( threadsNumber * sizeof(int));
    threadPacket = (Packet*) malloc( threadsNumber * sizeof(Packet));
    
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);
    

    //Connect to remote server
    for( i =0; i < threadsNumber; i++)
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
   			
     (read_size = recv(sock[0] , serverMessage , DEFAULT_BUFLEN , 0)); 
    
     if(read_size <= 0)
	{
		puts("Recieve failed");
		return 1;
	}
    
 
    threadsNumber = atoi(serverMessage);
    printf("Message recieved: %s\n",serverMessage);
   

    memset(available_files,0,sizeof(available_files));
    //Receive a message from client
     (read_size = recv(sock[0] , available_files , DEFAULT_BUFLEN , 0)); 
     if(read_size <= 0)
	{
		puts("Recieve failed");
		return 1;
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
    printf("Message recieved: %s\n",available_files);
    
   int size = atoi(available_files); 
   printf("SIZE IS %d\n",size); 
    

    
	pthread_t *tid =(pthread_t*) malloc(threadsNumber * sizeof(pthread_t));

    clock_t before = clock(), difference;

    for(i=0; i< threadsNumber; i++)
    {
    
		    
		threadPacket[i].socket = sock[i];
		if( i != (threadsNumber -1))
		     threadPacket[i].length = size/threadsNumber;
		else
		     threadPacket[i].length = size - (size / threadsNumber * (threadsNumber - 1)) + 2;
		
		argPacket= &threadPacket[i];

		pthread_create(&(tid[i]),NULL,&recievingThread,argPacket); 
	       
    }

        




    for(i= 0; i < threadsNumber; i++)
    {
	pthread_join(tid[i],NULL); 
    } 

    // making a file 
    int counter = 0;
    fp = fopen("DownloadedFile", "a");

    if (fp == NULL)
    {
    	printf("Can't open Downloaded File");
	return 1;
    
    }

    	while(counter < threadsNumber)
	{
		
		for( i = 0; i< threadsNumber; i++)
		{
			if(threadPacket[i].message[1] ==('0'+ counter))
			{
				fprintf(fp,"%s", &threadPacket[i].message[3]);
			}	
		
		}
		
		counter++;	
	}
    	
    
    
    
	fclose(fp);

	difference = clock() - before;
	int milliSeconds = difference * 1000/CLOCKS_PER_SEC;
	printf("I took me : %d seconds and %d milliseconds to download your file\n",milliSeconds /1000, milliSeconds %1000);

	
	FILE * plotFile = fopen("plot","a");

	fprintf(plotFile,"threads = %d  time: seconds: %d, milliseconds %d ;\n",threadsNumber,milliSeconds /1000, milliSeconds %1000);
    
	fclose(plotFile);
	for(i=0; i< threadsNumber ; i++)
    	close(sock[i]);
    
    return 0;
}

