#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    
    printf("sleeping %d ms before acquiring mutex\r\n",thread_func_args->wait_to_obtain_ms);
    usleep(1000*thread_func_args->wait_to_obtain_ms);
    
    printf("take mutex\r\n");
    pthread_mutex_lock(thread_func_args->mutex);

    printf("sleeping %d ms before releasing mutex\r\n",thread_func_args->wait_to_release_ms);
    usleep(1000*thread_func_args->wait_to_release_ms);

    printf("release mutex\r\n");
    pthread_mutex_unlock(thread_func_args->mutex);

    thread_func_args->thread_complete_success = true;
    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */

    // allocate a structure to be provided the the thread
    struct thread_data* p_data = malloc(sizeof(struct thread_data));
    p_data->wait_to_obtain_ms  = wait_to_obtain_ms;
    p_data->wait_to_release_ms = wait_to_release_ms;
    p_data->mutex              = mutex;

    // create the thread - the thread id will be returned in thread parameter
    if (pthread_create(thread,NULL,threadfunc,p_data) == 0)
    { 
        return true;
    }
    else
    {
        ERROR_LOG("Could not create thread : give-up");
        return false;
    }
}

