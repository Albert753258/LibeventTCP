#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <limits>

int main(){
    int soc = socket(AF_INET, SOCK_STREAM, 0);
    if(soc == -1){
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    printf("Please enter port number\n");
    int portNumber = 5555;
    std::cin >> portNumber;
    adr.sin_port = htons(portNumber);
    std::cin.ignore (std::numeric_limits<std::streamsize>::max(), '\n'); 
    printf("Please enter ip address\n");
    std::string ipAddr = "";
    std::getline(std::cin, ipAddr);
    int tmp = inet_pton(AF_INET, ipAddr.c_str(), &adr.sin_addr);
    if(tmp == 0){
        printf("Incorrect IP address\n");
        exit(EXIT_FAILURE);
    }
    else if(tmp == -1){
        perror("inet_pton failed\n");
        exit(EXIT_FAILURE);
    }
    if(connect(soc, (struct sockaddr *) &adr, sizeof adr) == -1){
        perror("Can't connect to server\n");
        exit(EXIT_FAILURE);
    }
    std::string cmd = "";
    while(true){
        cmd = "";
        printf("Please enter SQL command or exit\n");
        std::getline(std::cin, cmd);
        if(cmd == "exit"){
            exit(EXIT_SUCCESS);
        }
        int writeBytes = write(soc, cmd.c_str(), 25600);
        std::cout << "Write success\n";
        if(writeBytes == -1){
            perror("Write failed");
            close(soc);
            exit(EXIT_FAILURE);
        }
        char result[25600];
        ssize_t nread;
        nread = read(soc, result, 25600);
        if(nread <= 0){
            printf("Read error\n");
	    close(soc);
	    exit(EXIT_FAILURE);
        }
        std::cout << std::string(result) << "\n";
    }
}
