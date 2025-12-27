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
int proxySocketId; // main socket of our proxy, threads will be spawned from this socket.
// and every thread will get one socket [clientSocketId]

pthread_t tid[MAX_CLIENT];
sem_t semaphore;
pthread_mutex_t lock;

cache_element *head;
int cache_size;


int main(int argc, char * argv[])
{
    int clientSocketId, clientLength;
    struct sockaddr_in serverAdd , clientAdd;
    /*
    sockaddr_in is just a container that holds, IP & portnumber,
    it tell the OS where a socket should connect and listen on.
    */

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
    proxySocketId = socket(AF_INET,SOCK_STREAM, 0);// protocol = 0 -> chosen automatically.
    /*
    socket(x, y, z)
    x -> address family, IPv4, IPv6
    y -> socket type (TCP, UDP)
    z -> protocol
        socket() is a way of telling the OS, what kind of connection do I want.
        socket() gives me a handle to talk on the network.
        AF_INET => I want to talk on the internet (IPv4). 
        SOCK_STREAM => I want a TCP connection (like HTTP).
        0 => OS, choose the correct protocol for this, here since we have given SOCK_STREAM, it chooses TCP.
    final_result : 
    -   OS, give me a TCP socket for IPv4 internet communication.
    -   socket() creates a TCP network handle based on the rules you specify (x, y, z).
    */
    if(proxySocketId < 0)
    {
        perror("\tFailed to create a socket.\n");
        exit(1);
    }

    int reuse = 1;
    // to reuse the main proxyserver socket, we have to set socket options.
    if(setsockopt(proxySocketId, SOL_SOCKET, SO_REUSEADDR, (char*)reuse, sizeof(reuse)) < 0)
    {
        perror("\tsetsocketopt() failed, providing options to socket failed.\n");
        exit(1);
    }

    bzero((char*)&serverAdd, sizeof(serverAdd));// setting to NULL, as by default in C, structs hold garbage.
    serverAdd.sin_family = AF_INET;
    serverAdd.sin_port = htons(portNumber);
    serverAdd.sin_addr.s_addr = INADDR_ANY;// set any address for the server, that we will finally send the request to.
    if(bind(proxySocketId, (struct sockaddr*) &serverAdd, sizeof(serverAdd)) <0)
    {
        perror("\tBinding failed.\n");
        exit(1);
    }
    printf("\tBinding successful on port : %d\n", portNumber);


    int listenStatus = listen(proxySocketId, MAX_CLIENT);
    if(listenStatus < 0)
    {
        perror("Error while listening.\n");
        exit(1);
    }
    

    return 0;   
}