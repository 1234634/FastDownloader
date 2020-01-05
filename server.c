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

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27018
#define MAX_FILES 20
#define FILES_NUMBER 4

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
	int readSize;

	pthread_mutex_lock(&lock);

	if( send( sendingData->socket , sendingData->message , strlen(sendingData->message ) ,0 ) < 0 ) 
    		{
    			puts("Send failed");
			return NULL;	
   	 	}
		
	readSize = recv(sendingData->socket , recievedMessage , DEFAULT_BUFLEN , 0);
	printf("message recieved %s \n",recievedMessage);
	
	pthread_mutex_unlock(&lock);

	return NULL;
}


int fileSize(FILE* argFp)
{
	int size;
	fseek(argFp, 0, SEEK_END); // seek to end of file
	size = ftell(argFp); // get current file pointer
	fseek(argFp, 0, SEEK_SET); // seek back to beginning of file
	// proceed with allocating memory and reading the file
	printf("size is %d",size);
	
	return size;

}


void fileChunking(FILE * argFp, int argBegin, int argEnd, char (* argBuffer)[DEFAULT_BUFLEN])
{
	int length,bytesRead;
	if (argFp != NULL)    
	{	
  		// read up to sizeof(buffer) bytes
		fseek(argFp,argBegin,SEEK_SET);
		 length = argEnd - argBegin;
  		bytesRead = fread(*argBuffer, 1,length , argFp);
   		// process bytesRead worth of data in bufferi
		 printf("bytes red %d and what %s\n",bytesRead,*argBuffer);
  		
	}	

	
}

int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , read_size,threadsNumber,i;
    char client_message[DEFAULT_BUFLEN],availableFiles[DEFAULT_BUFLEN];
    char availableFilesArr[FILES_NUMBER][40] = {"Need for speed","Slenderman","JazzJackabit","Fifa08"};
    char fileName[30];
    FILE *fp; 
    struct sockaddr_in server ,client;
    
    
    if( argc < 2)
    {
    	threadsNumber = 3;
    }
    else
    {
    	threadsNumber = atoi(argv[1]);
    }
    
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
   
  if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

    while( (read_size = recv(client_sock , client_message , DEFAULT_BUFLEN , 0)) > 0 )
    {
   	printf("Bytes received: %d\n", read_size);
    	printf("Message recieved: %s\n",client_message);

	memset(availableFiles,0,sizeof(availableFiles));

	for( i=0; i <  FILES_NUMBER; i++)
	{	
		strcat(availableFiles,availableFilesArr[i]);
		strcat(availableFiles," ; ");
	}
    	if(send(client_sock, availableFiles, strlen(availableFiles),0) < 0) 
    	{
    		puts("Send failed");
		return 1;	
    	}
	
    	printf("Sending done \n ");

	memset(client_message,0,sizeof(client_message));
	(read_size = recv(client_sock , client_message , DEFAULT_BUFLEN , 0));//which file

	printf("message recieved %s\n",client_message);
	
	int num = atoi(client_message);
	memset(fileName, 0 , sizeof(fileName));
	strcpy(fileName,availableFilesArr[num]);
	printf("File name %s \n",fileName);
	
	fp = fopen(fileName, "r");

	if (fp == NULL)
	{
		printf("Requested file can't be sended");
		exit(1);
	}
	
	printf ("Odje");
	fileSize(fp);

	// split the file into buffers and put the number on each one 

	pthread_t *tid =(pthread_t*) malloc(threadsNumber * sizeof(pthread_t));
	Packet tempPacket;
	void ** arg_Packet = (void **) malloc(threadsNumber * sizeof(void*));
	int size = fileSize(fp),begin,end;
	//memset(tempPacket.message,0,30);
	fileSize(fp);

	for(i= 0; i < threadsNumber; i++)
	{	
	/*	begin = i*size/threadsNumber;
		if( i== threadsNumber-1)
			end = size;
		else
			end = (i+1)*size/threadsNumber;
	*/
		begin=0;
		end = 3;
		
		printf("Proso ovde %d", i);

	        fileChunking(fp,begin,end,&tempPacket.message);
		tempPacket.socket = client_sock;
		arg_Packet[i]= &tempPacket ;
	//	memset(tempPacket.message,0,sizeof(tempPacket));
		pthread_create(&(tid[i]),NULL,&sendingThread,arg_Packet[i]); 
	}

	for(i= 0; i < threadsNumber; i++)
	{
		pthread_join(tid[i],NULL); 
	} 
 	
	fclose(fp);
   	free(arg_Packet);
   	free( tid);
    }
 
   pthread_mutex_destroy(&lock); 
  

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

