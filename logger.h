
// This  header file for log creation                                        
// @Oussama AMARA                                                              
// Last modification 1/2/2023                                               
// version 1.0                                                               

#ifndef	_LOGGER_H
#define	_LOGGER_H	1
#if defined(_WIN32)//for windows
    #include "syslog-win32/syslog.h"
#else 
    #include <syslog.h>
#endif
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>

/**
 * @file logger.h
 * @brief  This is the header file containing all the function declarations to create  history(log) for both  client and  server .
 * @author oussama amara
 * @version 1.0
 * @date 31/1/2022
*/
#if defined(_WIN32)//for windows

#else 
    __BEGIN_DECLS
#endif

/*
 * Logging methods by levels
 */
void log_error(char* format, ...);
void log_warning(char* format, ...);
void log_status(char* format, ...);
void log_debug(char* format, ...);

/*
 * Log level configurator
 * Default is LOG_MAX_LEVEL_ERROR_WARNING_STATUS
 */ 

#define LOG_MAX_LEVEL_ERROR 0
#define LOG_MAX_LEVEL_ERROR_WARNING_STATUS 1
#define LOG_MAX_LEVEL_ERROR_WARNING_STATUS_DEBUG 2

void logger_set_log_level(const int level);

/*
 * Set target type
 * Default is syslog
 */
void logger_reset_state(void);
int logger_set_log_file(const char* filename, char* path);
void logger_set_out_stdout();

#if defined(_WIN32)//for windows

#else 
    __END_DECLS
#endif 

#endif /* _LOGGER_H.h  */