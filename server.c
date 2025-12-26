#include "proxy_parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_CLIENT 3

typedef struct cache_element cache_element;
struct cache_element
{
    char *data;
    int len;
    char *url; // we will be finding the element with the help of 'url'.
    time_t lru_time_track;
    // struct cache_element * next; // in C, we have to write struct to tell the data-type.
    // structs/classes in C++, the 'class_name' directly becomes the 'data-type'
    cache_element *next;
};

cache_element *findElement(char *url) {};
cache_element *addElement(char *data, char *url, int size) {};
void removeCacheElement(char *url) {};

int portNumber = 8080;
int proxySocketId;

pthread_t tid[MAX_CLIENT];
sem_t semaphore;
pthread_mutex_t lock;

cache_element *head;
int cache_size;


int main(int argc, char * argv[])
{
    sem_init(&semaphore, 0 /*minimum value*/, MAX_CLIENT /*maximum value of semaphore*/);// initialize the semaphore
    pthread_mutex_init(&lock, NULL);

    if(argc == 2)
    {
        portNumber = atoi(argv[1]);
        // ./proxy 8090 => port_number = 8090
    }
    else
    {
        printf("\tPort Number not given.\n");
        exit(1);
    }
    printf("\tStarting Proxy Server at port : %d\n", portNumber);
    return 0;   
}