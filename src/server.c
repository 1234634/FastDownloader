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
#include<pthread.h>
#include<stdlib.h>

#define DEFAULT_BUFLEN 20000
#define DEFAULT_PORT   27019
#define MAX_FILES 20
#define FILES_NUMBER 4
#define MESSAGE_ID "[%d]"
#define DEFAULT_FLEN 30
#define DEFAULT_THREADSNUM 10
#define T_SIZE 51

pthread_mutex_t lock;
typedef struct Packets
{
	int socket;
	char message[DEFAULT_BUFLEN];
	

}Packet;




void *sendingThread(void *arg_packet)
{
	Packet* sendingData = arg_packet;
	char recievedMessage[DEFAULT_BUFLEN];


	if( send( sendingData->socket , sendingData->message , strlen(sendingData->message ) ,0 ) < 0 ) 
    		{
    			puts("Send failed");
			exit(0);	
   	 	}
		
	if(recv(sendingData->socket , recievedMessage , DEFAULT_BUFLEN , 0) <= 0)
	{
		puts("Recieve failed");
		exit(0);
	}
	printf("message recieved %s \n",recievedMessage);
	

	return NULL;
}


int fileSize(FILE* argFp)
{
	int size;
	
	fseek(argFp, 0, SEEK_END); // seek to end of file
	size = ftell(argFp); // get current file pointer
	fseek(argFp, 0, SEEK_SET); // seek back to beginning of file
	
	return size;

}


void fileChunking(FILE * argFp, int argBegin, int argEnd, char (* argBuffer)[DEFAULT_BUFLEN])
{
	int length,idOffset = strlen(*argBuffer);
	length = argEnd - argBegin;
	
	if (argFp != NULL)    
	{	
  		fseek(argFp, argBegin, SEEK_SET);
  		fread((*argBuffer+idOffset), 1, length, argFp);
  		
	}	

	
}

int main(int argc , char *argv[])
{
    int socket_desc , *client_sock , c , read_size,threadsNumber,i;
    char client_message[DEFAULT_BUFLEN],availableFiles[DEFAULT_BUFLEN],serverMessage[DEFAULT_BUFLEN];
    char availableFilesArr[FILES_NUMBER][DEFAULT_BUFLEN] = {"NFS","Slenderman","JazzJackabit","Fifa08"};
    char fileName[DEFAULT_FLEN];
    FILE *fp; 
    struct sockaddr_in server ,client;
    
    
    if( argc < 2)
    {
    	threadsNumber = DEFAULT_THREADSNUM;
    }
    else
    {
    	threadsNumber = atoi(argv[1]);
    }
    
    client_sock = (int *) malloc( threadsNumber * sizeof(int));

    printf("Downloading threads number is %d ", threadsNumber);


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
    listen(socket_desc , threadsNumber);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    for( i=0; i < threadsNumber; i++){ 
	    
	    client_sock[i] = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	    
	    if (client_sock < 0)
	    {
		perror("accept failed ");
		return 1;
	    }

	    puts("Connection  accepted");
  
    }


  /////////////////////////////////////////////////////////////////////////////
   

    while( (read_size = recv(client_sock[0] , client_message , DEFAULT_BUFLEN , 0)) > 0 )
    {

	snprintf(serverMessage,sizeof(serverMessage),"%d",threadsNumber);
    	
	// Sending the number of threads 
	if(send(client_sock[0], serverMessage, sizeof(serverMessage), 0) < 0) 
    	{
    		puts("Send failed");
		return 1;	
    	}
    	printf("The number of threads: %d, has been sent \n",threadsNumber);
	
	memset(availableFiles, 0, sizeof(availableFiles));
	
	for( i=0; i <  FILES_NUMBER; i++)
	{	
		strcat(availableFiles,availableFilesArr[i]);
		if(i != FILES_NUMBER -1)
			strcat(availableFiles," ; ");
	}
	
	//Sending available files
    	if(send(client_sock[0], availableFiles, strlen(availableFiles),0) < 0) 
    	{
    		puts("Send failed");
		return 1;	
    	}
    	printf("Available files have been sent \n");

	//recieve which file does client want to download
	memset(client_message,0,sizeof(client_message));
	if( recv(client_sock[0] , client_message , DEFAULT_BUFLEN , 0)< 0)
 	{
		puts("Recieve failed");
		return 1;
	}

	printf("Client demanding file number: %s\n",client_message);
	

	int num = atoi(client_message);
	if( num < 0 || num > FILES_NUMBER)
	{
		printf("file number out of range \n");
		return 1;
	}	
	memset(fileName, 0 , sizeof(fileName));
	strcpy(fileName,availableFilesArr[num]);
	printf("File name %s \n",fileName);
	
	//opening the file
	fp = fopen(fileName, "r");

	if (fp == NULL)
	{
		printf("Requested file can't be sent");
		exit(1);
	}
 

	pthread_t *tid =(pthread_t*) malloc(threadsNumber * sizeof(pthread_t));
	Packet *tempPacket= (Packet *) malloc(threadsNumber* sizeof(Packet));
	void * argPacket;
	int size = fileSize(fp),begin,end;
	
	memset(serverMessage,0,strlen(serverMessage));
	sprintf(serverMessage,"%d",size);
	puts(serverMessage);
	// Sending msg length 
	if(send(client_sock[0], serverMessage, sizeof(serverMessage), 0) < 0) 
    	{
    		puts("Send failed");
		return 1;	
    	}
	
	for(i= 0; i < threadsNumber; i++)
	{	
		begin = i*size/threadsNumber;
		if( i== threadsNumber-1) //because size might not be dividable with thredsNumber
			end = size;
		else
			end = (i+1)*size/threadsNumber;
	

		snprintf(tempPacket[i].message, sizeof(tempPacket[i].message),MESSAGE_ID,i);
	        
		fileChunking(fp,begin,end,&tempPacket[i].message);
		
		tempPacket[i].socket = client_sock[i];
		argPacket= &tempPacket[i];

		pthread_create(&(tid[i]),NULL,&sendingThread,argPacket); 
	}

	for(i= 0; i < threadsNumber; i++)
	{
		pthread_join(tid[i],NULL); 
	} 
 	
	fclose(fp);
   	free(tempPacket);
   	free( tid);
    }
 
  

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

