
// This  header file for log creation                                        
// @Oussama AMARA                                                              
// Last modification 31/1/2023                                               
// version 1.0                                                               

#ifndef	_LOGGER_H
#define	_LOGGER_H	1
#include <syslog.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>

/**
 * @file logger.h
 * @brief 
 * @author oussama amara
 * @version 1.0
 * @date 31/1/2022
*/

__BEGIN_DECLS

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
int logger_set_log_file(const char* filename);
void logger_set_out_stdout();

__END_DECLS

#endif /* _LOGGER_H.h  */