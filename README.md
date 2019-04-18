# GarrisonServerClient
A concurrent server-client system coding test for Garrison Technology

To compile the system go into the source folder from the topfolder using
cd src

After this you must compile both the server.cpp and the client.cpp file
To do this use the following two lines of code in the terminal:
g++ -o server server.cpp -lssl -lcrypto -lpthread
g++ -o client client.cpp

To run the system in seperate terminal type first:
./server <portnumber>

Followed by:
./client <portnumber>
  
Note: the portnumber must be the same for the client and server to be able to connect correctly
This has been tested on a linux operating system using a g++ combiler only.
