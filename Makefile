
# Application 
app :server.o   Client.o   arg_test.o
	gcc server.o arg_test.o  -o Server 
	gcc Client.o arg_test.o  -o Client  
	 
#serveur

Server.o:server.c
	gcc -c server.c

#Client

Client.o:Client.c
	gcc -c Client.c

#test argument of shell

arg_test.o:arg_test.c
	gcc -c arg_test.c

clean:
#cleanup all object file
	  -rm *.o $(objects) 
	  -rm  Server Client