# FastDownloader
Downloader that downloads fast.  
If you are using 2 raspberry pies in first type:
sudo make server_clean
sudo make server
./server arg1

In second one:
sudo make client_clean
sudo make client
./client arg1 arg2

arg1- threadsNumber arg2-FileNumber

First type when in src folder: chmod u+x makeAndRun  
chmod u+x compile  
That way it will provide rights to these scripts   
To compile the programms use type : ./compile in src folder   
To compile and run with default settings use : ./makeAndRun  
If you want to use arguments first one represents number of threads and second is the file chosing option(0 -3).
Example: ./makeAndRun 5 3
This example will run 5 threads to download file number 3.

 
 

  

