# FastDownloader
Downloader that downloads fast.  

First type when in src folder: chmod u+x makeAndRun  
chmod u+x compile
Thats way it will provide rights to these scripts 
To compile the programms use type : ./compile in src folder   
To compile and run use : ./makeAndRun  
*Note: Compiler must work with threads thus you have to change compile comand.  
*Go to the makefile.server and in line 21 add  -pthread after gcc .  


What should client do:  
1) Send a start message it is redudant what should be in it;  
2) Recieve the number of threads from server   
3) Recieve available files string  
4)Parse it  
5) Offer a menu to user so he can chose which file to download  
6)Send number of file which should be downloaded (it is recommended to have some protection so it will only send number in range of amout of files)  
7)Start a timer that will measure download spped  
8)recieve the content from threads and while recieving send a confirmation message  
9)stop the timer when every thread has sent it's content  
10)print the information about downloading speed inot a file and plot it   
11)close(sock) - this sends info to the server that the client hase terminated so he can do the same.   

  

