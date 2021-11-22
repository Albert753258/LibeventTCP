#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include "sql.cpp"
#include <iostream>
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/thread.h>
#include <signal.h>

SQLUtils sql = SQLUtils("localhost", "root", "1", "testDB", 3306);
int serverSoc = 0;


void onMessage(int clientSoc, short tmp1, void *arg){
    char buf[25600];
    ssize_t nread;
    struct event *msgEvent= (struct event*)arg;
    nread = read(clientSoc, buf, 25600);
    if(nread <= 0){
        printf("Read error\n");
	event_free(msgEvent);
	std::cout << "Free success\n";
	close(clientSoc);
	printf("Exiting thread\n");
	pthread_exit(NULL);
	printf("Exit thread\n");
        return;
    }
    if(std::string(buf) != ""){
        cout << buf << '\n';
        std::string result = sql.sendSQL(std::string(buf));
        int nwrite = write(clientSoc, result.c_str(), 25600);
        if(nread <= 0){
            printf("Write error\n");
	    event_free(msgEvent);
	    close(clientSoc);
	    printf("Exiting thread\n");
	    pthread_exit(NULL);
	    printf("Exit thread\n");
            return;
        }
    }
}

void *message(void *arg){
    struct event *msgEvent = event_new(NULL, -1, 0, NULL, NULL);
    struct event_base *base;
    base = event_base_new();
    //int clientSoc = *((int*)&arg);
    //std::cout << clientSoc << "\n";
    //printf(*((int *)arg));
    event_assign(msgEvent, base, *((int*)&arg), EV_READ|EV_WRITE|EV_PERSIST, onMessage, (void*)msgEvent);
    event_add(msgEvent, NULL);
    event_base_dispatch(base);
    return NULL;
}

void onAccept(int serverSoc, short events, void *arg){
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSoc = accept(serverSoc, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if(clientSoc == -1){
        perror("Socket attach failed");
        exit(EXIT_FAILURE);
    }
    int reuseaddr_on = 1;
    setsockopt(clientSoc, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on, sizeof(reuseaddr_on));
    pthread_t thread;
    pthread_create(&thread, NULL, message, (void*)clientSoc);
    pthread_detach(thread);
}


void shutdownServer(int s){
    if(serverSoc != 0){
        close(serverSoc);
    }
    exit(0);
}


int main(){
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = shutdownServer;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);


    serverSoc = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSoc == -1){
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port = htons(5555);
    if(bind(serverSoc, (struct sockaddr *) &adr, sizeof adr) == -1){
        perror("Socket binding failed");
        exit(EXIT_FAILURE);
    }
    if(listen(serverSoc, 5) == -1){
        perror("Socket listening failed");
        exit(EXIT_FAILURE);
    }
    struct event_base *base;
    base = event_base_new();
    struct event acceptEvent;
    event_assign(&acceptEvent, base, serverSoc, EV_READ|EV_WRITE|EV_PERSIST, onAccept, base);
    event_add(&acceptEvent, NULL);
    event_base_dispatch(base);
}
