 
// This  is  the  code for the  server                                       
// ./Server -p Port_NBR  -d  Folder_Path                                       
// @Oussama AMARA                                                              
// Last modification 31/1/2023                                                 
// version 0.5                                                                
// @open issue : +color for  printf

/**
 * @file server.c
 * @brief  This is  the  server  side, it  should  run all time  waiting  for  client  to connect. The server has two arguments "-p" followed  by the port number to use  for this server then "-d" followed  by the  path of the  directory to share.
 * @author oussama amara
 * @version 0.5
 * @date 11/1/2022
*/
#include <arpa/inet.h> // inet_addr()/
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>//system
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()
#include <errno.h>
#include "Const.h"
#include "arg_test.h"
#include <time.h>//get time and date for log file 
#include <string.h>//


void  get_time()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	int year = 0, month  = 0, day  = 0, h = 0,mn = 0,sec  = 0;

    year = tm.tm_year + 1900 ;
	month = tm.tm_mon + 1;
	day = tm.tm_mday;
	h=tm.tm_hour ;
	mn =tm.tm_min;
	sec=tm.tm_sec;

	printf("now: %d-%02d-%02d %02d:%02d:%02d   \n",year,month,day,h, mn, sec);

}
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
    printf("[i]Connected to Clent: %s:%d\n",inet_ntoa(cli.sin_addr),ntohs(cli.sin_port));
    write(*ptr_connfd, fname,256);

    FILE *fp = fopen(fname,"rb");
    if(fp==NULL)
    {
		printf("\033[0;31m");
        printf("[-]File for transfer  open error\n");
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
	printf("[i]size of the file to transfer is  %f Kb\n",(res/1024.0));
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
		    	printf("[+]File transfer completed for id: %d\n",*ptr_connfd);
				printf("\033[0m");
			}
            if (ferror(fp))
            {   
				printf("\033[0;31m");
				printf("[-]Error reading file for  transfer \n");
				printf("\033[0m");
			}
            break;
        }
    }
	printf("\033[0;32m");
	printf("[+]Closing Connection for id: %d\n",*ptr_connfd);
	printf("\033[0;31m");
	close(*ptr_connfd);
	//shutdown(*ptr_connfd,SHUT_WR);
	sleep(2);
}

/**
 * @brief   The function uses the  code send by the client to know  what it  should  do, if  there's  no code send then the function returns 1. If  the code is "list" this function will get the list of  file and folder in the  shared folder using the  script the  output  will redirect to text file. If the  code  is "transmit" then the code uses the name of  file, send by the client then start transmit it  to the client.  . It has two parameters  " *ptr_connfd  " and  "  **argv ". "  **argv ".
 * @param[in]  **argv  Pointer to pointer  contain  the name  and the  argument  such as the  path to the  folder to share.
 * @param[in]  *ptr_connfd  Pointer serve as the channel  between the client  and the server, aka all transmitted data can be read using this variable .
 * @return void
 * 
*/
// Function designed for chat between client and server.
int share_msg(int* ptr_connfd,char **argv ,struct sockaddr_in cli,int *ptr_change )
{
	FILE  *filePointer,*filePointerTransfer ;

	char cmd [100]="./script.sh  ";
	char buff[MAX];
	char string[MAX];
	int Exist =0   ;
	int  j =0;

	int err ;
	pthread_t tid; 
	// bzero :Set N bytes of pointer to 0.   
	bzero(buff, MAX);//or you can use memset (&buff,'\0',sizeof(buff))

	// read the message from client and copy it in buffer
	read(*ptr_connfd, buff, sizeof(buff));
	// print buffer which contains the client contents
	
	if (strncmp("bash_list", buff, 9) == 0) 
	{   printf("[i]The client wants information about shared folder\n ");
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
		bzero(buff, MAX);
		read(*ptr_connfd, buff, sizeof(buff));
		/*********************************************///must change dir if it run 2nd it  crush 
		char path[100] ;
		//get the current  dir path  
		
		if (*ptr_change)
		{
			getcwd(path, sizeof(path)) ;
		}
		*ptr_change =0 ;
		int dir_test = chdir(path);
		//printf(" hello dir_test =%d and path=%s\n",dir_test,path);
		//system("pwd");//test to check  dir path
		/********************************************/
        filePointer = fopen("stdout.txt", "r");
        if (filePointer == NULL)
        {
			printf("\033[0;31m");
            printf("[-]Error can t find list of file to share \n");
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
		printf("\033[0m");
		exit(1);
	}
	else
	{	
		printf("\033[0;32m");
		printf("[+]Socket successfully created..\n");
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
		printf("\033[0m");
		exit(1);
	}
	else
	{	
		printf("\033[0;32m");
		printf("[+]Socket successfully binded..\n");
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
		printf("\033[0m");
		exit(1);
	}
	else
	{	
		printf("\033[0;32m");
		printf("[+]Server listening..\n");
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
			printf("\033[0m");
			exit(1);
		}
		else
		{
			printf("\033[0;32m");
			printf("[+]server accept the client...\n");
			printf("\033[0m");
		}
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
	
	int change =1 ; //for transfer 
	int *ptr_change =&change;
	check_arg_server(argc,argv);
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
		int b = share_msg(ptr_connfd,argv,cli,ptr_change);
		if (!b)
		{
			close(connfd);
			break;
		}
		close(connfd);
        sleep(1);	
		
	}
	// After chatting close the socket
	close(sockfd);
	printf("[i] Server will Shutdown  \n");
	return 0;	
}
