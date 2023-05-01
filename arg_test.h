
// This  header file for argument test                                        
// @Oussama AMARA                                                              
// Last modification 2/2/2023                                               
// version 0.5                                                                

#ifndef	_ARG_TEST_H
#define	_ARG_TEST_H	1
#if defined(_WIN32)//for windows
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0600
    #endif
    #include <winsock2.h>
    #include <windows.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define bzero(b,len) (memset((b), '\0', (len)), (void) 0)  
#else//Mac and linux
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <errno.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>// opendir 
#include <ctype.h>
#include "Const.h"
/**
 * @file arg_test.h
 * @brief This is the header file containing all the function declarations to verify the argument for the server and the client.
 * @author oussama amara
 * @version 0.5
 * @date 11/1/2022
*/
#if defined(_WIN32)//for windows

#else 
    __BEGIN_DECLS
#endif
void check_dir (char **argv);

int isNumber(char number[]);

int isvalidport(int a);

int isValidIpAddress(char *ipAddress);

int validateIP4Dotted(const char *s);

void check_arg_client(int argc, char **argv  );

int check_arg_server(int argc, char **argv  );

#if defined(_WIN32)//for windows

#else 
    __END_DECLS
#endif 

#endif /* arg_test.h  */


