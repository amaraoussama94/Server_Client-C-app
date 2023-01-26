 
 
# PIKA_PIKA  
 
## Project veriosn  : 
  0.5 

## Project skeleton
.

├── Headers

├── Sources

├── libs

├── scripts

├── doc

├── README.md

## Key features

The real platform:  Linux

Few dependencies (socket , )
## Project overview :  

-The aim of this application is to make two PC (aka server-client) connect using  socket  to share  a directory or  transfer file .

-Basically, we will have  a server and  client, we  will use TCP/IP standard for this project with IP V4.

![ALT](/Server-based-network.png "Server-based-network")


![ALT](/Socket_server_flowchart%20.png "flowchart")

*The entire process can be broken down into following steps:

*TCP Server 

-using create(), Create TCP socket. 

-using bind(), Bind the socket to server address.

-using listen(), put the server socket in a passive mode, where it waits for the client to approach the server to make a connection.

-using accept(), At this point, connection is established between client and server, and they are ready to transfer data.

-Using  share_msg ()  the  server  keeps  waiting for  specific msg  from  client like a simple code  to know  what he  should do,  we  talk  about  choose  between to list  the  shared folder  files, transfer  a specific  file  or  just do not  any thing   just     connect if  the client  don't send  any code. The  last one is made to test the connection, if  any problem  happens with e  list or  transfer  files option.

-Go back to Step 3.

*TCP Client

-Create TCP socket.

-connect newly created client socket to server.

-Using share_msg (), here the  client  will send a  specific code  to the  server, it  depends  if the user wants  to list or share or  just connect to the server. After  sending the  code the client will wait  for  a response from the  server and share it to the user.

PS :For the  test of different  arguments for  both  server  and the client  are done using the arg_test lib.

## Installation
-install make :
```
$ sudo apt install make
```
-clone project :
```
git clone  http://gitlab.sofia.local/dsw/dsem/training/file-transfer.git
```
or  just download  it  as  a  zipe file  using the  button in the image : 

![ALT](/download.png " download " )

unzip the file using :
```
unzip file-transfer.zip
```
```
- cd /file-transfer
```
```
-make 
```
## Usage

First run the server 
```
. /Server -p PORT_NBR  -d PATH_FOLDER_TO_SHARE 
```
-For port number: you can use their 8080 to avoid any problem, please try number higher than 2000.

-PATH_FOLDER_TO_SHARE: this is the complete  path  for the folder  to share.

-Then run the  client:
```
./Client -ip  @_IP -p PORT_NUM OPTION
```
For the @_IP: if you  run the client and  the server on the same machine (locally) you can use 127.0.0.1  else if the  server and the client  run on different machine, please  use the  @IP of the  machine that the  server runs on it (try: IP address).

- For the port address, please  use the  same port of the  server.

-Option:

You can pass no option, this is used  to test if there's  a connection between the  server and the client.

Else  you  can try "--list ": all the folder and the file in the  folder  shared  by the  server  will be listed 

Else  try "-T  FILE_NAME": T stand for transfer file, so  you need to pass the name of the  file to transfer from the server to you. It must be one of the  files in the  shared folder.

## Project status

This is the first part  of the project .(done  waiting  for review )

## Useful link 
-[socket-programming-cc](https://www.geeksforgeeks.org/socket-programming-cc/)

-[tcp-server-client-implementation-in-c](https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/)  

-[socket-programming](https://www.cs.dartmouth.edu/~campbell/cs50/socketprogramming.html)

-[socket](https://pubs.opengroup.org/onlinepubs/009619199/socket.htm) 
-[Adding Color to Your Output From C](https://www.theurbanpenguin.com/4184-2/)