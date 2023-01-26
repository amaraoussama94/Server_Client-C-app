 
//  This  is  the  code for the  Client                                        
//   ./Client -ip @IP -p Port_NBR  --list (option)                             
//   ./Client -ip @IP -p Port_NBR  --T (option) path(optional with T)          
// @Oussama AMARA                                                              
// Last modification 26/1/2023                                                 
// version 0.5                                                                 
// @open issue : nan

#include <arpa/inet.h> // inet_addr()/
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
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

void share_msg(int *sockfd,char ** argv)
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
		printf("[i]This is the list of file and  folder shared by the server : \n These are the directories  : \n" );
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
		bzero(msg, sizeof(msg));
		strcat(msg,argv[6]);
		write(*sockfd, msg, sizeof(msg));
		//check if the file name exist or no
		bzero(msg, sizeof(msg));
		read(*sockfd, msg, sizeof(msg));
		if (strcmp(msg,"file_error")==0)
		{	printf("\033[0;31m");
			printf("[-]Please try  again ,maybe try --list first then try again ,or maybe no file has this name \n" );
			printf("\033[0m");
			exit(1);
		}	

		//get the  file 
		char* dirname = "transfer";
		mkdir(dirname,0755);
		//system("mkdir transfer");
		chdir("transfer");
		//system("cd test/");
		//system("pwd");
		filetransferPointer = fopen(argv[6] , "ab");
		if (filetransferPointer == NULL)
		{
			printf("\033[0;31m");
			printf("[-]Error  can t  create file for transfer  \n");
			printf("\033[0m");
			exit(-1);
		}
		/**********************************************/
		char size[10];
		long double size_file =0;
		read(*sockfd, size, sizeof(size));
		//convert data to  long double
		sscanf(size, "%Lf", &size_file);
		printf("[i]Size of the file %Lf Kb \n",size_file);
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
			usleep(20000);//sleep for 20ms
			//printf("%s \n", recvBuff);
		}
		printf("\n");

		if(bytesReceived < 0)
		{
			printf("\n Read Error \n");
		}
		printf("[i] file transmission is done you can check the file    \n");

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
		printf("\033[0;31m");
		exit(1);
	}
	else
	{	printf("\033[0;32m");
		printf("[+]Socket successfully created..\n"); 
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
		printf("\033[0m");
		exit(1);
	}
	else
	{
		printf("\033[0;32m");
		printf("[+]connected to the server..\n");
		printf("\033[0m");
	}

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

	//check argument 
	check_arg_client(argc,argv);
	
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
		share_msg(ptr_sockfd,argv);
	}
		
	// close the socket
	close(sockfd);
}
