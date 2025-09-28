/**
 * @file platform_thread.c
 * @brief Cross-platform thread abstraction for client handling.
 *        Uses pthreads on Linux/macOS and CreateThread on Windows.
 * @author Oussama Amara
 * @version 1.0
 * @date 2025-08-15
 */

#include "platform_thread.h"

#ifdef _WIN32
#include <windows.h>

int create_thread(thread_t* thread, THREAD_FUNC (*func)(void*), void* arg) {
    // Create a new thread using Windows API
    *thread = CreateThread(NULL, 0, func, arg, 0, NULL);
    return (*thread != NULL) ? 0 : -1;
}

void detach_thread(thread_t thread) {
    CloseHandle(thread);
}

#else
#include <pthread.h>

int create_thread(thread_t* thread, THREAD_FUNC (*func)(void*), void* arg) {
    return pthread_create(thread, NULL, func, arg);
}

void detach_thread(thread_t thread) {
    pthread_detach(thread);
}
#endif
