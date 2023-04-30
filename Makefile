
# Application 
app_windows  :server.c   Client.c   arg_test.c logger.c
	gcc server.c logger.c arg_test.c  -o Server  -lws2_32  
#	gcc Client.c  logger.c arg_test.c  -o Client  
app_linux :server.o   Client.o   arg_test.o logger.o
	gcc server.o logger.o arg_test.o  -o Server 
	gcc Client.o  logger.o arg_test.o  -o Client  
	 
#serveur

server.o:server.c
	gcc -c server.c

#Client

Client.o:Client.c
	gcc -c Client.c

#test argument of shell

arg_test.o:arg_test.c
	gcc -c arg_test.c

# create log  file 
#Client

logger.o:logger.c
	gcc -c logger.c

clean:
#cleanup all object file
	  -rm *.o $(objects) 
	  -rm  Server Client