 
// This  is  the  code for the  server                                       
// ./Server -p Port_NBR  -d  Folder_Path   
// ./Server --History                                   
// @Oussama AMARA                                                              
// Last modification 30/4/2023                                                 
// version 0.5                                                                
// @open issue : +color for  printf
//               +windows support for logs 
//               + update documentation

/**
 * @file server.c
 * @brief  This is  the  server  side, it  should  run all time  waiting  for  client  to connect. The server has two arguments "-p" followed  by the port number to use  for this server then "-d" followed  by the  path of the  directory to share.
 * @author oussama amara
 * @version 0.5
 * @date 11/1/2022
*/

//_WIN32  for  windows  system 
//else it run for mac os and linux  system


#if defined(_WIN32)//for windows
    #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0600
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
	#define bzero(b,len) (memset((b), '\0', (len)), (void) 0) 
	#include <windows.h> 
#else//Mac and linux
#include "logger.h"//for log file 
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <errno.h>
#endif
#include <stdio.h>
#include <stdlib.h>//system
#include <string.h>
#include <string.h>//
#include "Const.h"
#include "arg_test.h"



/**
 * @brief This  function sends the file to the client we  talk about text , video , image ,pdf ..files  . It has three  parameters  " cli  " ," fname " and  "  connfd ".
 * @param[in]  *fname  pointer to the  file  name that  we  will send to the client .
 * @param[in]  cli client struct  , we need it  to get the Ipadress of the  client who connect to our  server
 * @param[in]  *connfd  Pointer serve as the channel  between the client  and the server, aka all transmitted data can be read using this variable .
 * @return void
 * 
*/
//void* SendFileToClient(int *arg)
void SendFileToClient(int *ptr_connfd,char * fname ,struct sockaddr_in cli )
{
	
    //int connfd=(int)*arg;
    printf("[i]Connection accepted and id: %d\n",*ptr_connfd);
    printf("[i]Connected to Client: %s:%d\n",inet_ntoa(cli.sin_addr),ntohs(cli.sin_port));
	#if defined(_WIN32)//for windows
	#else
		log_status("Connected to Client: %s:%d",inet_ntoa(cli.sin_addr),ntohs(cli.sin_port));
	#endif
	write(*ptr_connfd, fname,256);

    FILE *fp = fopen(fname,"rb");
    if(fp==NULL)
    {
		printf("\033[0;31m");
        printf("[-]Can't open file %s  for transfer \n",fname);
		#if defined(_WIN32)//for windows
		#else
			log_error("Can't open file %s  for transfer ",fname);
		#endif
		printf("\033[0m");
        exit(1);   
    }   
	/*******************************************/
	fseek(fp, 0L, SEEK_END);
  
    // calculating the size of the file
    long int res = ftell(fp);
	//conver to char to send it 
	char size[10];
  	sprintf(size, "%f", (res/1024.0) );
	//send file size to  the client 
	write(*ptr_connfd, size, sizeof(size));
	printf("[i]Size of the file %s to transfer is  %f Kb\n",fname,(res/1024.0));
	#if defined(_WIN32)//for windows
	#else
		log_status("Size of the file %s to transfer is  %f Kb",fname,(res/1024.0));
	#endif
	fseek(fp, 0, SEEK_SET);
    /******************************************/
        /* Read data from file and send it */
    while(1)
    {
        /* First read file in chunks of 256 bytes */
        unsigned char buff[1024]={0};
        int nread = fread(buff,1,1024,fp);
        //printf("Bytes read %d \n", nread);        

        /* If read was success, send data. */
        if(nread > 0)
        {
            //printf("Sending \n");
            write(*ptr_connfd, buff, nread);
        }
        if (nread < 1024)
        {
            if (feof(fp))
			{

               // printf("End of file\n");
				printf("\033[0;32m");
		    	printf("[+]File %s transfer completed for id: %d\n",fname,*ptr_connfd);
				#if defined(_WIN32)//for windows
				#else
					log_status("File %s transfer completed for id: %d",fname,*ptr_connfd);
				#endif

				printf("\033[0m");
			}
            if (ferror(fp))
            {   
				printf("\033[0;31m");
				printf("[-]Error reading file %s for  transfer \n",fname);
				#if defined(_WIN32)//for windows
				#else
					log_error("Error reading file %s for  transfer ",fname);
				#endif	
				printf("\033[0m");
			}
            break;
        }
    }
	printf("\033[0;32m");
	printf("[+]Closing Connection for id: %d\n",*ptr_connfd);
	#if defined(_WIN32)//for windows
	#else
		log_status("Closing Connection for id: %d",*ptr_connfd);
	#endif
	printf("\033[0;31m");
	close(*ptr_connfd);
	//shutdown(*ptr_connfd,SHUT_WR);
	#if defined(_WIN32)//for windows
		Sleep(2); 
	#else
		sleep(2);
	#endif
}

/**
 * @brief   The function uses the  code send by the client to know  what it  should  do, if  there's  no code send then the function returns 1. If  the code is "list" this function will get the list of  file and folder in the  shared folder using the  script the  output  will redirect to text file. If the  code  is "transmit" then the code uses the name of  file, send by the client then start transmit it  to the client.  . It has two parameters  " *ptr_connfd  " and  "  **argv ". "  **argv ".
 * @param[in]  **argv  Pointer to pointer  contain  the name  and the  argument  such as the  path to the  folder to share.
 * @param[in]  *ptr_connfd  Pointer serve as the channel  between the client  and the server, aka all transmitted data can be read using this variable .
 * @return void
 * 
*/
// Function designed for chat between client and server.
int share_msg(int* ptr_connfd,char **argv ,struct sockaddr_in cli,char* path)
{
	FILE  *filePointer,*filePointerTransfer ;

	char cmd [100]="./script.sh  ";
	char buff[MAX];
	char string[MAX];
	int Exist =0   ;
	int  j =0;

	int err ;
	//pthread_t tid; 
	// bzero :Set N bytes of pointer to 0.   
	bzero(buff, MAX);//or you can use memset (&buff,'\0',sizeof(buff))

	// read the message from client and copy it in buffer
	read(*ptr_connfd, buff, sizeof(buff));
	// print buffer which contains the client contents
	
	if (strncmp("bash_list", buff, 9) == 0) 
	{   printf("[i]The client wants information about shared folder\n ");
		#if defined(_WIN32)//for windows
		#else
			log_status("The client wants information about shared folder");
		#endif
		//| awk  '{print $1 " " $11 }'  " " : field are sparate with space , $1 et $ 11 print colom1 and 11
		//chdir(argv[4]);// change the path 
		strcat(cmd,argv[4]);
		//int status = system("ls -al|awk '{print $1 " " $11}' > cmdoutput" ); 
		int status = system(cmd); 
		//open file 
		filePointer = fopen("stdout.txt", "r");
		//ste the psotion of the  pointer at the bening of the file
		if (filePointer == NULL)
			{
				printf("\033[0;31m");
				printf("[-]Error can t find list of file to share \n");
				#if defined(_WIN32)//for windows
				#else
					log_error("Error can t find list of file to share");
				#endif
				
				printf("\033[0m");
				bzero(buff,sizeof(buff));
				strcat(buff,"ERRORFILE1");
				write(*ptr_connfd, buff, sizeof(buff));
				exit(1);
			}
		fseek(filePointer, 0, SEEK_SET);
		while(fgets(buff, MAX, filePointer)) 
		{   
		
			//   send that buffer to clients
			
			if (strncmp(buff ,".",1)==0 ||  strncmp(buff ,"..",2)==0  ||  strncmp(buff ,"/",1)==0)
			{
				continue;
			}
			
			write(*ptr_connfd, buff, sizeof(buff));
			
		}

		bzero(buff, MAX);
		for(int j =0 ;j<sizeof(buff);j++)
		{
			buff[j] =  '#';
		}
		//send to client 					
		write(*ptr_connfd, buff, sizeof(buff));
		//file 
		fclose(filePointer);

		return 1;
	}
	else if (strcmp("bash_Transf", buff) == 0) 
	{
		int num_file ; 
		//get numlber of  file to send 
		bzero(buff, MAX);
		read(*ptr_connfd, buff, sizeof(buff));
		sscanf(buff,"%d",&num_file);
		/********************///////////////////////////////////////////////////////*/
		printf("[i] the  number of file to send to the client is %d\n",num_file);
		//get file name 
		bzero(buff, MAX);
		read(*ptr_connfd, buff, sizeof(buff));
		//printf("the file name is %s \n",buff);

		//must change dir if it run 2nd it  crush 
		int dir_test = chdir(path);
		//printf(" hello dir_test =%d and path=%s\n",dir_test,path);
		//system("pwd");//test to check  dir path
		/********************************************/
        filePointer = fopen("stdout.txt", "r");
        if (filePointer == NULL)
        {
			printf("\033[0;31m");
            printf("[-]Error can t find list of file to share \n");
			#if defined(_WIN32)//for windows
			#else
			log_error("Error can t find list of file to share ");
			#endif
			printf("\033[0m");
            exit(1);
        }
		//get iof the file name  existt or no in the shared folder 
        while (  fgets( string ,MAX, filePointer) )// fscanf(filePointer,"%s", string) == 1
        {
			//lets take out the \n  introduce by fgets 
			string[strcspn(string,"\n")]= 0;
			
			if (strcmp(string,buff)==0)
			{
				Exist = 1;
				break;
			}	
        }
		if (Exist)
		{   
			fclose(filePointer);
			//transfer file here 
			chdir(argv[4]);// change the path 
			printf( "[i]the file for transfer  name is = %s \n",buff);
			#if defined(_WIN32)//for windows
			#else
				log_status("the file for transfer  name is = %s ",buff);
			#endif
			
			SendFileToClient(ptr_connfd,buff ,cli );
			//create thread  for sending the file to the client 
			/*err = pthread_create(&tid, NULL, &SendFileToClient, ptr_connfd);
			if (err != 0)
				printf("\ncan't create thread :[%s]", strerror(err));*/
			 
		}
		else
		{
			
			bzero(buff, sizeof(buff));
			strcat(buff,"file_error");
			write(*ptr_connfd, buff, sizeof(buff));
			bzero(buff, sizeof(buff));
			printf("\033[0;31m");
			printf("[-] Client try to get file that doesn't exist in the  shared folder \n");
			#if defined(_WIN32)//for windows
			#else
				log_warning("Client try to get file that doesn't exist in the  shared folder");
			#endif
			
			printf("\033[0m");
		}

		// si il n y a  aucun paramatere passe au clien ni t ni
		// list le  server se ferme  par  return 1 and close the while loop
		return 1;
	}
	else 
		return 0;
	
} 
/**
 * @brief This  function will create the socket as TCP using IPV4. It has one  parameter  " *sockfd   ".
 * @param[in]  *sockfd  Pointer container of port number  and  address of the  server  .
 * @return void
 * 
*/
void create_scoket(int * sockfd)
{
	*sockfd = socket(AF_INET, SOCK_STREAM, 0);// AF_INET :IPV4  , SOCK_STREAM:TCP
	if (sockfd < 0) 
    {
		printf("\033[0;31m");
		perror("[-]socket creation failed...\n");
		#if defined(_WIN32)//for windows
		#else
			log_error("socket creation failed...");
		#endif
		
		printf("\033[0m");
		exit(1);
	}
	else
	{	
		printf("\033[0;32m");
		printf("[+]Socket successfully created..\n");
		#if defined(_WIN32)//for windows
		#else
				log_status("Socket successfully created..");
		#endif

		printf("\033[0m");
	}
	
}
/**
 * @brief This  function will bind the port  number and the  address. It has two parameters  " servaddr  " and  "  sockfd ".
 * @param[in]  *servaddr  pointer to structure  that contains the port number  and  address of  the server .
 * @param[in]  *sockfd  Pointer container of port number  and  address of the  server  .
 * @return void
 * 
*/

void socket_bind(struct sockaddr_in *servaddr ,int * sockfd)
{
	
	// Binding newly created socket to given IP and verification
	if ((bind(*sockfd, (SA*)servaddr, sizeof(*servaddr))) < 0) 
	{
		printf("\033[0;31m");
		perror("[-]socket bind failed...\n");
		#if defined(_WIN32)//for windows
		#else
			log_error("socket bind failed...");
		#endif
		
		printf("\033[0m");
		exit(1);
	}
	else
	{	
		printf("\033[0;32m");
		printf("[+]Socket successfully binded..\n");
		#if defined(_WIN32)//for windows
		#else
			log_status("Socket successfully binded..");
		#endif
		
		printf("\033[0m");
	}
}

/**
 * @brief This  function listens up to 5 connection requests. It has one  parameter  " sockfd  ".
 * @param[in]  *sockfd  Pointer container of port number  and  address of the  server  .
 * @return void
 * 
*/
void socket_listening (int *sockfd)
{
	if ((listen(*sockfd, LISTENQ)) != 0) //5 connection requests will be queued before further requests are refused.
	{
		printf("\033[0;31m");
		printf("[-]Listen failed...\n");
		#if defined(_WIN32)//for windows
		#else
			log_error("Listen failed...");
		#endif
		
		printf("\033[0m");
		exit(1);
	}
	else
	{	
		printf("\033[0;32m");
		printf("[+]Server listening..\n");
		#if defined(_WIN32)//for windows
		#else
			log_status("Server listening..");
		#endif

		printf("\033[0m");
	}
}
/**
 * @brief This  function accept client connection . It has for parameters  " sockfd  " , " cli "and  " ," len " connfd ".
 * @param[in]  *cli  client structure which  contains  all info about the client   .
 * @param[in]  *len  pointer length  of  the client structure  .
 * @param[in]  *sockfd  Pointer container of port number  and  address of the  server  
 * @param[in]  *connfd  Pointer serve as the channel  between the client  and the server, aka all transmitted data can be read using this variable .
 * @return void
 * 
*/

void socket_accept (int *connfd , int* sockfd ,struct sockaddr_in*cli , int *len)
{
	*connfd = accept(*sockfd, (SA*)cli, len);
			
		if (connfd < 0) 
		{
			printf("\033[0;31m");
			perror("[-]server accept failed...\n");
			#if defined(_WIN32)//for windows
			#else
				log_error("server accept failed...");
			#endif
			
			printf("\033[0m");
			exit(1);
		}
		else
		{
			printf("\033[0;32m");
			printf("[+]server accept the client :%s:%d \n",inet_ntoa(cli->sin_addr),ntohs(cli->sin_port));
			#if defined(_WIN32)//for windows
			#else
				log_status("server accept the client :%s:%d",inet_ntoa(cli->sin_addr),ntohs(cli->sin_port));
			#endif
			
			printf("\033[0m");
		}
}
int  win_socket_init()
{
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d)) //initialize Winsock ,MAKEWORD macro allows us to request Winsock version 2.2
    {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }  
}
void win_socket_cleanup()
{
	#if defined(_WIN32)
        WSACleanup();
    #endif
}
// Driver function 
/**
 * @brief This  function  main function will return 0 if it run succefuly. It has two parameters  " argc  " and  "  **argv ".
 * @param[in]  argc  number  of arguments  passed to the server +1 (name of the script) .
 * @param[in]  **argv  Pointer to pointer  contain  the name  and the  argument  such as the  path to the  folder to share. .
 * @return int 0 for  success 
 * 
*/
int main(int argc, char **argv)
{
	
	//for socket
	int sockfd, connfd, len;
	int *ptr_sockfd ,*ptr_connfd , *ptr_len;
	struct sockaddr_in servaddr, cli;
	struct sockaddr_in *ptr_servaddr , *ptr_cli;
	char folder_path[250] ;
	FILE * logfilePointer = NULL ;
	char ligne[1024];
	
	const char log_file_name [50]= "log.txt";
    //must check best position 
	#if defined(_WIN32)
		win_socket_init();
	#endif
	//check argument 
	check_arg_server(argc,argv);

	if(strcmp("--History", argv[1]) == 0 )
	{
		FILE * logfilePointer = NULL ;
		logfilePointer = fopen("log.txt", "r");
		//ste the psotion of the  pointer at the bening of the file
		if (logfilePointer == NULL)
			{
				printf("\033[0;31m");
				printf("[-]Error can t find log   file   \n");
				printf("\033[0m");
				exit(1);
			}
		fseek(logfilePointer, 0, SEEK_SET);
		while(fgets(ligne, MAX, logfilePointer)) 
		{   
			//print only server log
			if(strncmp("Server:",ligne,7) == 0 )
			{	
				printf("%s ",ligne);
			}
			
		}

		fclose(logfilePointer);
		return 0;
	}

	//get the current  dir path  
	bzero(folder_path,sizeof(folder_path));
	getcwd(folder_path, sizeof(folder_path)) ;
	//printf(" the path is %s \n",folder_path);
 
	
	#if defined(_WIN32)//for windows
	#else
		// reset all to log  file
		logger_reset_state();
		// open file for create log 
		logger_set_log_file(log_file_name,folder_path);
	#endif

	
	// socket create and verification
	ptr_sockfd = &sockfd;
	create_scoket(ptr_sockfd);
	//initialize to zeo 
    // bzero :Set N bytes of pointer to 0.  
	bzero(&servaddr, sizeof(servaddr));//or you can use memset (&servaddr,'\0', sizeof(servaddr))

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr =htonl(INADDR_ANY);// Address to accept any incoming messages.   
	servaddr.sin_port = htons(atoi(argv[2]));//htons :Functions to convert between host and network byte order
	//binding
	ptr_servaddr = &servaddr;
	socket_bind(ptr_servaddr ,ptr_sockfd);

	// Now server is ready to listen and verification
	socket_listening (ptr_sockfd);
	//chek for ctr+z as input to stope server	
	while (1)
	{
		
		len = sizeof(cli);
		//printf("hello ok \n");
		// Accept the data packet from client and verification
		ptr_connfd = &connfd;
		ptr_len = &len;
		ptr_cli= &cli ;
		socket_accept (ptr_connfd , ptr_sockfd , ptr_cli , ptr_len);
 
		//temp sol to stop server 
		int b = share_msg(ptr_connfd,argv,cli,folder_path);
		if (!b)
		{
			close(connfd);
			break;
		}
		close(connfd);
        #if defined(_WIN32)//for windows
			Sleep(2); 
		#else
			sleep(2);
		#endif
		
		
	}
	//cleaning all and close  log file  using :  cleanup_internal();
	// After chatting close the socket
	close(sockfd);
	printf("[i] Server will Shutdown  \n");
	#if defined(_WIN32)//for windows
	#else
		log_warning("Server will Shutdown ");
	#endif
	
	//must  chek bes position 
	win_socket_cleanup();
	return 0;	
}
 