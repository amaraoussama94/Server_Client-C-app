 
//  This  is  the  code for argument   shell test                              
// @Oussama AMARA                                                              
// Last modification  30/01/2023                                                
// version 1.0                                                                 
 // @open issue : nan

#include "arg_test.h"


/**
 * @file arg_test.c
 * @brief  It contains all functions necessary for checking the assigned argument for the server and client..
 * @author oussama amara
 * @version 0.5
 * @date 11/1/2022
*/

/**
 * @brief same as  linux ones ...//TODO
 * @param[in] ......
 * @return int 
 * 
*/
#if defined(_WIN32)//for windows
	int Inet_pton(int af, const char *src, void *dst)
	{
	struct sockaddr_storage ss;
	int size = sizeof(ss);
	char src_copy[INET6_ADDRSTRLEN+1];

	ZeroMemory(&ss, sizeof(ss));
	/* stupid non-const API */
	strncpy (src_copy, src, INET6_ADDRSTRLEN+1);
	src_copy[INET6_ADDRSTRLEN] = 0;

	if (WSAStringToAddress(src_copy, af, NULL, (struct sockaddr *)&ss, &size) == 0) {
		switch(af) {
		case AF_INET:
		*(struct in_addr *)dst = ((struct sockaddr_in *)&ss)->sin_addr;
		return 1;
		case AF_INET6:
		*(struct in6_addr *)dst = ((struct sockaddr_in6 *)&ss)->sin6_addr;
		return 1;
		}
	}
	return 0;
	}
#endif
/**
 * @brief This function will check if the server shared directory path is valid or not. It has a "**argv" parameter..
 * @param[in]  **argv Pointer to pointer contains the name and the argument such as the path of the folder to share.
 * @return void
 * 
*/

void check_dir_serv (char **argv)
{
	DIR* dir = opendir(argv[4]);
	if (dir) 
	{
		printf("\033[0;32m");
    	printf("[+]The directory  has been open successfully\n");
		printf("\033[0m");
    	closedir(dir);
	} 
	#if defined(_WIN32)//for windows
		else 
	#else
		else if (ENOENT == errno) 
	#endif
	{
		printf("\033[0;31m");
    	perror("[-]Please enter a valid path should be / ...\n");
		printf("\033[0m");
		exit(1);
	}
}

/**
 * @brief This function will check if the    directory path is valid or not This path will serve as  download path. It has a "**argv" parameter..
 * @param[in]  **argv Pointer to pointer contains the name and the argument such as the path of the folder to share.
 * @return void
 * 
*/
void check_dir_client (char **argv)
{
	DIR* dir = opendir(argv[6]);
	if (dir) 
	{
		printf("\033[0;32m"); //green
    	printf("[+] directory open sucess \n");
		printf("\033[0m");
    	closedir(dir);
	} 
	#if defined(_WIN32)//for windows
		else 
	#else
		else if (ENOENT == errno) 
	#endif
	{
		printf("\033[0;31m");
    	perror("[-]Please enter a valid path should be / ...\n");
		printf("\033[0m");
		exit(1);
	}
}

/**
 * @brief This feature checks if the port number is availed integer or not, it has a parameter "number [] ".
 * @param[in]  Table number [] which includes the port number.
 * @return int  will be 0 or  1 , as  a boolien .
 * 
*/
//check argument is number or no
int isNumber(char number[])
{
    int i = 0;

    //checking for negative numbers
    if (number[0] == '-')
        return False;
    for (; number[i] != 0; i++)
    {
        //if (number[i] > '9' || number[i] < '0')
        if (!isdigit(number[i]))
            return False;
    }
    return True;
}

/**
 * @brief THis  function check that the port number is higher than 1024 (for Linux  system). It has one parameter  " a  "  .
 * @param[in]  a integer is the port number   .
 * @return int   Will return an error code  if the port is less than 1024
 * 
*/
//check if port number is > 1023 
int isvalidport(int a)
{
	if ( a < 1024)
	{
		printf("\033[0;31m");
		printf("[-]Please enter a valide port number , it must be sup then 1023\n");
		printf("\033[0m");
		exit(1);     

	}
}

/**
 * @brief This  function checks if the passed address of the client is a valid IP address. It has one parameter  " *ipAddress ".
 * @param[in]  *ipAddress A pointer to the  buffer that conteient the IP address .
 * @return int Returns 0 if the IP address is correct. 
 * 
*/
//check @IP V4
int isValidIpAddress(char *ipAddress)
{
    struct sockaddr_in sa;
	
	#if defined(_WIN32)//for windows
		int result = Inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
	#else 
    	int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
	#endif
    return result  ;
}

/**
 * @brief This  function checks if the passed address of the client is a valid IP address but manuelle (just back up solution not used ). It has one parameter  " *s ".
 * @param[in]  *s A pointer to the  buffer that conteient the IP address .
 * @return int Returns 1 if the IP address is correct. 
 * 
*/
//Manuelle check   @IP V4
int validateIP4Dotted(const char *s)
{
    int len = strlen(s);

    if (len < 7 || len > 15)
        return 0;

    char tail[16];
    tail[0] = 0;

    unsigned int d[4];
	
    int c = sscanf(s, "%3u.%3u.%3u.%3u %s", &d[0], &d[1], &d[2], &d[3], tail);
    if (c != 4 || tail[0])
	{
        return 0;

	}	

    for (int i = 0; i < 4; i++)
    {
		if (d[i] > 255)
		{
			return 0;
		}

	}

    return 1;
}

/**
 * @brief This  function will check all the passed argument to the client, will check the order and option passed if there re valid one or  no and the  argument for each option using some  function. It has two parameters  " argc  " and  "  **argv ".
 * @param[in]  Argc  its contain a number of passed arguments to client +1 (name of the file).
 * @param[in]  **argv Pointer to pointer contains the name and the argument such as the path of the folder to share.
 * @return void
 * 
*/
void check_arg_client(int argc, char **argv  )
{
	if ( argc < 5 )//|| argc > 7
	{
		printf("[i]./Client -ip @IP -p Port_NBR \nwith two option\n");
		printf("[i]./Client -ip @IP -p Port_NBR --list\n");
		printf("[i]./Client -ip @IP -p Port_NBR -T (for transfer file)  file_name_to_transf(1)  file_name_to_transf(2) .. \n");
		exit(1);

	}
	
	if  (strcmp("-ip", argv[1]) != 0) 
	{
		printf("\033[0;31m");
		printf("[-]please try again available option are \"-ip \"and -p only...\n");
		printf("\033[0m");
		printf("[i]./Client -ip @IP -p Port_NBR\n");
		exit(1);

	}
	if  (strcmp("-p", argv[3]) != 0) 
	
	{
		printf("\033[0;31m");
		printf("[-]please try again available option are -ip and\" -p \"only...\n..\n");
		printf("\033[0m");
		printf("[i]./Client -ip @IP -p Port_NBR\n");
		exit(1);

	}

	if ((5 < argc ))
	{
		if  ((strcmp("--list", argv[5]) != 0)  && (strcmp("-T", argv[5]) != 0)  )
		{
			printf("\033[0;31m");
			printf("[-]please try again available option are -ip , -p  and \"--list\" \"-T\" only...\n..\n");
			printf("\033[0m");
			printf("[i]./Client -ip @IP -p Port_NBR --list\n");
			printf("[i]./Client -ip @IP -p Port_NBR -T (for transfer file)  file_name_to_transf(1)  file_name_to_transf(2) .. \n");
			exit(1);

		}

		if (strcmp("-T", argv[5]) == 0) 
		{
			if  ( argc < 7)/*******************************/
			{
				printf("[i]./Client -ip @IP -p Port_NBR -T (for transfer file)  file_name_to_transf(1)  file_name_to_transf(2) ..  \n");
				exit(1);

			}

		}
		if (strcmp("--list", argv[5]) == 0) /**************************/
		{
			if  ( argc > 6)
			{
				printf("[i]./Client -ip @IP -p Port_NBR --list\n");
				exit(1);

			}

		}
	}
	if (isNumber(argv[4]) == False)
	{
		printf("\033[0;31m");
		printf("[-]invalid port number please use digit only\n");
		printf("\033[0m");
		exit(1);
	}
	isvalidport(atoi(argv[4]));
	if (!validateIP4Dotted(argv[2]) )//    !isValidIpAddress( argv[2]))
	{
		printf("\033[0;31m");
		printf("[-]invalid IP adress V4 please try again \n");
		printf("\033[0m");
		exit(1);
	}
}

/**
 * @brief This  function will check all the passed argument to the server , will check the order and option passed if there re valid one or  no and the  argument for each option using some  function. It has two parameters  " argc  " and  "  **argv ".
 * @param[in]  Argc  its contain a number of passed arguments to server +1 (name of the file).
 * @param[in]  **argv Pointer to pointer contains the name and the argument such as the path of the folder to share.
 * @return int 
 * 
*/

int check_arg_server(int argc, char **argv  )
{
	if ( argc < 2|| argc > 5)/*****************change it */
	{
		printf("[i]./Server -p Port_NBR -d folder_path\n");
		printf("[i]./Server --History\n");

		exit(1);			 

	}
	if  ((strcmp("-p", argv[1]) != 0) && (strcmp("--History", argv[1]) != 0) )
	{
		printf("\033[0;31m");
		printf("[-]please try again available option are --History , -p and -d \n");
		printf("\033[0m");
		printf("[i]./Server -p Port_NBR -d folder_path\n");
		printf("[i]./Server --History\n");
		printf("test  1\n");
		exit(1);

	}
	if ((strcmp("--History", argv[1]) == 0 )&& (argc >2))
	{
		printf("\033[0;31m");
		printf("[-]please try again available option are History , -p and -d \n");
		printf("\033[0m");
		printf("[i]./Server -p Port_NBR -d folder_path\n");
		printf("[i]./Server --History\n");
		printf("test  2\n");
		exit(1);
	}
	else if ((strcmp("--History", argv[1]) == 0 )&& (argc == 2))
	{
		
		return 1; // back to code all ok 
	}
	if ((strcmp("-p", argv[1]) == 0 )&& (argc <5))
	{
		printf("\033[0;31m");
		printf("[-]please try again available option is -p and -d \n");
		printf("\033[0m");
		printf("[i]./Server -p Port_NBR -d folder_path\n");
		exit(1);
	}
	
	if  (strcmp("-d", argv[3]) != 0) 
	{
		printf("\033[0;31m");
		printf("[-]please try again available option is -p and -d \n");
		printf("\033[0m");
		printf("[i]./Server -p Port_NBR -d folder_path\n");
		exit(1);

	}
	if  ((strcmp("..", argv[4]) == 0) ||(strcmp(".", argv[4]) == 0) )
	{
		printf("\033[0;31m");
		printf("[-]please try again .Access denied to this folder  \n");
		printf("\033[0m");
		exit(1);

	}
	//int status = system("./foo 1 2 3"); //run bash script
	if (isNumber(argv[2]) == False)
	{
		printf("\033[0;31m");
		printf("[-]invalid port number please use digit only\n");
		printf("\033[0m");
		exit(1);
	}
	isvalidport(atoi(argv[2]));
	check_dir_serv (argv);
	
}