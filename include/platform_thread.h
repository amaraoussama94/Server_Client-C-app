/**
 * @file platform_thread.h
 * @brief Cross-platform threading abstractions.
 *       Provides a unified interface for thread creation and management on Windows and POSIX systems.
 *      Supports C++17 standard.
 * @date 2024-09-14
 * @author Oussama Amara
 * @version 1.0
 */
#ifndef PLATFORM_THREAD_H
#define PLATFORM_THREAD_H

#ifdef _WIN32
#include <windows.h>
typedef HANDLE thread_t;
#define THREAD_FUNC DWORD WINAPI
#define THREAD_RETURN return 0
#else
#include <pthread.h>
typedef pthread_t thread_t;
#define THREAD_FUNC void*
#define THREAD_RETURN return NULL
#endif

/**
 * @brief Creates a new thread.
 * @param thread Pointer to store the created thread handle.
 * @param func Function to be executed by the thread.
 * @param arg Argument to be passed to the thread function.
 * @return 0 on success, -1 on failure.
 */
int create_thread(thread_t* thread, THREAD_FUNC (*func)(void*), void* arg);
/**
 * @brief Detaches a thread, allowing it to run independently.
 * @param thread The thread to detach.
 * @return void
 */
void detach_thread(thread_t thread);

#endif
