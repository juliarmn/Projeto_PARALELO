#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <math.h>

#define PORT_MANAGER "8080"

void check_error(const int code, const char *func_name)
{
    if (code == -1)
    {
        perror("func_name");

        exit(1);
    }
}

int main()
{
    struct addrinfo hints, *res;
    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    hints.ai_flags = AI_PASSIVE;
    int err = getaddrinfo(NULL, PORT_MANAGER, &hints, &res);
    if (err != 0)
    {
        fprintf(stderr, "getaddrinfo: %s", gai_strerror(err));
        exit(1);
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    check_error(sockfd, "socket()");

    err = bind(sockfd, res->ai_addr, res->ai_addrlen);
    check_error(err, "bind()");
    err = listen(sockfd, 1);
    check_error(err, "listen()");

    int clientfd = accept(sockfd, NULL, NULL);
    check_error(err, "accept()");

    uint32_t network_number;
    ssize_t bytes;

    bytes = recv(sockfd, &network_number, sizeof(network_number), 0);
    printf("O resultado da redução é: %d\n", network_number);
    close(sockfd);
    close(clientfd);
    freeaddrinfo(res);

    return 0;
}