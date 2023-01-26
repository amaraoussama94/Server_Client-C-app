
// This  header file for argument test                                        
// @Oussama AMARA                                                              
// Last modification 11/1/2023                                               
// version 0.5                                                                

#ifndef	_ARG_TEST_H
#define	_ARG_TEST_H	1
#include <ctype.h>
#include <arpa/inet.h>  
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "Const.h"
#include <errno.h>
#include <dirent.h>// opendir 

/**
 * @file arg_test.h
 * @brief This is the header file containing all the function declarations to verify the argument for the server and the client.
 * @author oussama amara
 * @version 0.5
 * @date 11/1/2022
*/

__BEGIN_DECLS

void check_dir (char **argv);

int isNumber(char number[]);

int isvalidport(int a);

int isValidIpAddress(char *ipAddress);

int validateIP4Dotted(const char *s);

void check_arg_client(int argc, char **argv  );

void check_arg_server(int argc, char **argv  );

__END_DECLS

#endif /* arg_test.h  */


