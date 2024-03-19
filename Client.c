 
//  This  is  the  code for the  Client                                        
//   ./Client -ip @IP -p Port_NBR  --list (option)                             
//   ./Client -ip @IP -p Port_NBR  --T   Files_Names     
//   ./Client -History     
// @Oussama AMARA                                                              
// Last modification 2/2/2023                                                 
// version 0.5                                                                 
// @open issue :+ for less then 1kb file the  loading  file  bave  go so wired 
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
	#include <strings.h>// bzero()
	#include"logger.h"
#endif
#include <stdio.h>
#include <stdlib.h>//system
#include <string.h>
#include "Const.h"
#include "arg_test.h"

// for mkdir function 
#include <sys/stat.h>
#include <sys/types.h>



const int PROG_BAR_LENGTH =30 ;//30 caractere
/**
 * @file client.c
 * @brief  This is the client side, after connecting to the server, using the passed argument the client will send the spacial code to the server to get the needed info.
 * @author oussama amara
 * @version 0.5
 * @date 11/1/2022
*/



/**
 * @brief This function will show  a progress bar that will be updated accordingly  percentage of  receiving a file,  this is not 100% accurate.
 * @param[in] percent_done this is the  percentage that will be usied to  move  progress bar  and  show it in shell
 * @return void
 */




void update_bar(float  percent_done  )
{	 
    int num_car =(int) percent_done * PROG_BAR_LENGTH  / 100 ;//number  of caractere to print 
    printf(" \r[");
    for(int i =0;i<num_car;i++)
    {
        printf("\033[0;32m"); //Set the text to the color Green
        printf("■");
    }
    for(int i =0;i<(PROG_BAR_LENGTH -num_car);i++) //unfinish part of  progress bar 
    {
        printf(" ");
    }
	if((int)percent_done > 100)
	{
		percent_done =100 ;
	}
    printf("\033[0m"); //Resets the text to default color
    printf("] %d%% Done.",(int)percent_done );
   // fflush(stdout);//print all to the  screen  
}



/**
 * @brief printing an ANSI escape code - the ones used to control tty behavior (clear screen, move cursor, etc.)
 * @param[in] x is the column of the terminal.
 * @param[in] y is the row of the terminal.
 * @return void
 */
void gotoxy(int x,int y)
{
	//The first character interpreted is 0x1B, which is equivalent to 27 in decimals, 
	//and is translated as the escape character (ESC) in the ASCII coding system. 
	//The rest now becomes an ANSI escape sequence and thus, is used for cursor positioning.

	printf("%c[%d;%df",0x1B,y,x);
}
 

/**
 * @brief  The function uses the argument  input    to know  what code  should send to the server, if  there's  no option (one of --list or -T) there's no code to send. If --list passed as an argument   the code is "bash__list" will be sent to the server side. If -T is passed as an argument  the code  "bash__transmit" will be sent to the server. It has two parameters  " *ptr_connfd  " and  "  **argv ". "  **argv ".
 * @param[in]  **argv  Pointer to pointer  contain  the name  and the  argument  such as the  path to the  folder to share.
 * @param[in]  *ptr_connfd  Pointer serve as the channel  between the client  and the server, aka all transmitted data can be read using this variable .
 * @return void
 * 
*/

void share_msg(int *sockfd,char ** argv, int argc)
{
	char buff[MAX]="bash";
	char msg[MAX];
	
	FILE *filetransferPointer;
	int n , bytesReceived = 0 ;

	if  (strcmp("--list", argv[5]) == 0)
	{
		strcat(buff,"_list");
	}
	if (strcmp("-T", argv[5]) == 0) 
	{
		strcat(buff,"_Transf");
	}
	//send to  server
	write(*sockfd, buff, sizeof(buff));
	bzero(buff, sizeof(buff));
	if  (strcmp("--list", argv[5]) == 0)
	{
		//read receive msg from server
		read(*sockfd, msg, sizeof(msg));
		read(*sockfd, msg, sizeof(msg));
		if(!strcmp(msg,"ERRORFILE1"))
		{
			printf("\033[0;31m");
			printf("[-] Error server can t list file \n ");
			printf("\033[0m");
			#if defined(_WIN32)//for windows
			#else
				log_error("Error server can t list file");
			#endif
			exit(1);
		}
		printf("[i]This is the list of file and  folder shared by the server : \n These are the directories  : \n" );
		#if defined(_WIN32)//for windows
		#else
			log_status(" liste the content of the  shared folder");
		#endif
		while(1)
		{
			read(*sockfd, msg, sizeof(msg));
			
			if(!strncmp(msg, "#", 1) )
			{
				break;

			}
			printf("%s \n ",msg);
		} 
	}
	else if (strcmp("-T", argv[5]) == 0) 
	{
		//get number of file to receive from server 
		int num_file = argc-6;
		bzero(msg, sizeof(msg));
		sprintf(msg,"%d",num_file);
		write(*sockfd, msg, sizeof(msg));
		//printf ("num = %s\n",msg);
		//send file name to server 
		bzero(msg, sizeof(msg));
		strcat(msg,argv[6]);
		printf(" the  buff is = %s",msg);
		write(*sockfd, msg, sizeof(msg));
		//check if the file name exist or no
		bzero(msg, sizeof(msg));
		read(*sockfd, msg, sizeof(msg));
		if (strcmp(msg,"file_error")==0)
		{	printf("\033[0;31m");
			printf("[-]Please try  again ,maybe try --list first then try again ,or maybe no file has this name \n" );
			#if defined(_WIN32)//for windows
			#else
				log_warning("You try to get  a file that not mentioned in the list");
			#endif
			printf("\033[0m");
			exit(1);
		}	

		//get the  file 
		char* dirname = "transfer";
		#if defined(_WIN32)//for windows
			mkdir(dirname);
		#else
			mkdir(dirname,0755);
		#endif
		
		//system("mkdir transfer");
		chdir("transfer");
		//system("cd test/");
		//system("pwd");
		filetransferPointer = fopen(argv[6] , "a+");
		if (filetransferPointer == NULL)
		{
			printf("\033[0;31m");
			printf("[-]Error  can t  create file %s for transfer  \n",argv[6]);
			#if defined(_WIN32)//for windows
			#else
			#endif
			
			printf("\033[0m");
			exit(-1);
		}
		/**********************************************/
		char size[10];
		long double size_file =0;
		read(*sockfd, size, sizeof(size));
		//convert data to  long double
		sscanf(size, "%Lf", &size_file);
		printf("[i]Size of the file %s %Lf Kb \n",argv[6],size_file);
		#if defined(_WIN32)//for windows
		#else
			log_status("Size of the file %s %Lf Kb ",argv[6],size_file);
		#endif
		
		/*************************************************/
		
		/* Receive data in chunks of 256 bytes */
		long double sz=0;
		long double * ptr_sz =&sz;
		while((bytesReceived = read(*sockfd, msg, 1024)) > 0)
		{ 
			//sz++;
			//gotoxy(0,4);
			//printf(" \r Received: %LF Mb \t \t \t",(sz/1024)); //LF for long double
			//fflush(stdout);
			// recvBuff[n] = 0;
			/******************************************************/
			  double percent_done;
			//long double  batch_size =1/1024 ;//Mb
			percent_done += 100/ size_file ; //;(batch_size/size_file)*100
			//printf(" the file  size is %Lf the percent_done = %lf  and in in = %d \n",size_file,percent_done,(int)percent_done);
			update_bar(percent_done);
			fflush(stdout);
			fwrite(msg, 1,bytesReceived,filetransferPointer);
			#if defined(_WIN32)//for windows
				Sleep(1); 
			#else
				usleep(20000);//sleep for 20ms
			#endif
			
			//printf("%s \n", recvBuff);
		}
		printf("\n");

		if(bytesReceived < 0)
		{
			printf("\033[0;31m");
			printf("[-]Error  can t  create file %s to recive it  \n",argv[6]);
			printf("\033[0m");
			#if defined(_WIN32)//for windows
			#else
				log_error("Error  can t  create file %s to recive it  ");
			#endif
			
		}
		fclose(filetransferPointer);
		printf("\033[0;32m");
		printf("[i] file transmission %s is done you can check transfer folder   \n",argv[6]);
		printf("\033[0m");
		#if defined(_WIN32)//for windows
		#else
			log_status("file transmission %s is done you can check transfer folder   ",argv[6]);
		#endif
		

	}
			
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
		printf("\033[0;31m");
		exit(1);
	}
	else
	{	printf("\033[0;32m");
		printf("[+]Socket successfully created..\n");
		#if defined(_WIN32)//for windows
		#else
			log_status("Socket successfully created.."); 
		#endif
		
		printf("\033[0m");
	}
	
}


/**
 * @brief This  function will try to connect to the server . It has two    parameters    " *sockfd   " and  " *servaddr ".
 * @param[in]  *sockfd  Pointer container of port number  and  address of the  server  .
 ** @param[in]  *servaddr  pointer to structure  that contains the port number  and  address of  the server .
 * @return void
 * 
*/

void socket_connect(struct sockaddr_in *servaddr ,int * sockfd)
{
	
	if (connect(*sockfd, (SA*)servaddr, sizeof(*servaddr)) <0) 
    {
		printf("\033[0;31m");
		perror("[-]connection with the server failed...\n");
		#if defined(_WIN32)//for windows
		#else
			log_error("connection with the server failed...");
		#endif
		printf("\033[0m");
		exit(1);
	}
	else
	{
		printf("\033[0;32m");
		printf("[+]connected to the server.. :%s:%d \n",inet_ntoa(servaddr->sin_addr),ntohs(servaddr->sin_port));
		#if defined(_WIN32)//for windows
		#else
			log_status("connected to the server.. :%s:%d",inet_ntoa(servaddr->sin_addr),ntohs(servaddr->sin_port));
		#endif
		
		printf("\033[0m");
	}

}

//TODO doc 
int  win_socket_init()
{
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d)) //initialize Winsock ,MAKEWORD macro allows us to request Winsock version 2.2
    {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }  
}
//TO DO Doc 

void win_socket_cleanup()
{
	#if defined(_WIN32)
        WSACleanup();
    #endif
}
/**
 * @brief This  function  main function will return 0 if it run succefuly. It has two parameters  " argc  " and  "  **argv ".
 * @param[in]  argc  number  of arguments  passed to the server +1 (name of the script) .
 * @param[in]  **argv  Pointer to pointer  contain  the name  and the  argument  such as the  path to the  folder to share. .
 * @return int 0 for  success 
 * 
*/

int main(int argc, char **argv)
{
	int sockfd, connfd;
	int *ptr_sockfd;
	struct sockaddr_in servaddr, client_addr;
	struct sockaddr_in *ptr_servaddr;

	char folder_path[250] ;
	const char log_file_name [50]= "log.txt";//same as  server  for now

	FILE * logfilePointer = NULL ;
	char ligne[1024];
	
	//check argument 
	
	#if defined(_WIN32)//for windows
		win_socket_init();
	#endif
	check_arg_client(argc,argv);
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
			//print only client log
			if(strncmp("Client:",ligne,7) == 0 )
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
		// oen file for create log 
		logger_set_log_file(log_file_name,folder_path);
	#endif
	
	// socket create and verification
	ptr_sockfd= &sockfd;
	 create_scoket(ptr_sockfd);
        //initialize to zeo 
        // bzero :Set N bytes of pointer to 0.  
	bzero(&servaddr, sizeof(servaddr));//or you can use memset (&servaddr,'\0', sizeof(servaddr))
	// assign IP, PORT   
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[2]);// the adress of the serveur 
	servaddr.sin_port = htons(atoi(argv[4]));  //htons :Functions to convert between host and network byte order
	// connect the client socket to server socket
	ptr_servaddr = &servaddr ;
	socket_connect(ptr_servaddr ,ptr_sockfd);
 
  	// run the lisst or transfer  option 
	if( argc > 5)
	{
		share_msg(ptr_sockfd,argv, argc);
	}
		
	// close the socket
	close(sockfd);
	printf("[i] Connection with the server is closed \n");
	#if defined(_WIN32)//for windows
	#else
		log_warning("Connection with the server is closed");
		logger_reset_state();
	#endif
	//must  chek bes position 
	win_socket_cleanup();
	return 0 ;
}
