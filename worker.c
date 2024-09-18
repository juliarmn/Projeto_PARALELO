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

#define PORT_WORKER_1 "8081"
#define PORT_WORKER_2 "8082"
#define PORT_WORKER_3 "8083"
#define PORT_WORKER_4 "8084"
#define PORT_WORKER_5 "8085"
#define PORT_WORKER_6 "8086"
#define PORT_WORKER_7 "8087"
#define PORT_WORKER_8 "8088"
#define PORT_MANAGER "8080"
#define NUM_WORKERS 8

void check_error(const int code, const char *func_name)
{
    if (code == -1)
    {
        perror("func_name");

        exit(1);
    }
}

void calculate_port(int num, char *port)
{
    switch (num)
    {
    case 1:
        strcpy(port, PORT_WORKER_1);
        break;
    case 2:
        strcpy(port, PORT_WORKER_2);
        break;
    case 3:
        strcpy(port, PORT_WORKER_3);
        break;
    case 4:
        strcpy(port, PORT_WORKER_4);
        break;
    case 5:
        strcpy(port, PORT_WORKER_5);
        break;
    case 6:
        strcpy(port, PORT_WORKER_6);
        break;
    case 7:
        strcpy(port, PORT_WORKER_7);
        break;
    case 8:
        strcpy(port, PORT_WORKER_8);
        break;
    default:
        fprintf(stderr, "Número de worker inválido (deve ser entre 1 e 8).\n");
        exit(1);
        break;
    }
}

void *server_function(int *num, char *PORT)
{
    struct addrinfo hints, *res;
    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    hints.ai_flags = AI_PASSIVE;
    int err = getaddrinfo(NULL, PORT, &hints, &res);
    if (err != 0)
    {
        fprintf(stderr, "getaddrinfo: %s", gai_strerror(err));
        exit(1);
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    check_error(sockfd, "socket()");

    err = bind(sockfd, res->ai_addr, res->ai_addrlen);
    check_error(err, "bind()");
    err = listen(sockfd, 8);
    check_error(err, "listen()");

    int clientfd = accept(sockfd, NULL, NULL);
    check_error(err, "accept()");

    uint32_t network_number;
    ssize_t bytes;
    bytes = recv(sockfd, &network_number, sizeof(network_number), 0);

    *num += ntohl(network_number);

    uint32_t num_sum = htonl(*num);
    send(clientfd, &num_sum, sizeof(num_sum), 0);
    close(sockfd);
    close(clientfd);
    freeaddrinfo(res);
}

void *client_function(int *num, char *PORT)
{
    struct addrinfo hints, *res;
    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int err = getaddrinfo("127.0.0.1", PORT, &hints, &res);

    if (err != 0)
    {
        fprintf(stderr, "getaddrinfo: %s", gai_strerror(err));
        exit(1);
    }

    int sockfd = socket(res->ai_family,
                        res->ai_socktype,
                        res->ai_protocol);
    check_error(sockfd, "socket()");

    err = connect(sockfd, res->ai_addr, res->ai_addrlen);
    check_error(err, "connect()");

    uint32_t network_number = htonl(*num);

    ssize_t bytes_sent = send(sockfd, &network_number, sizeof(network_number), 0);

    if (bytes_sent < 0)
    {
        perror("send()");
        exit(EXIT_FAILURE);
    }
    ssize_t bytes;

    bytes = recv(sockfd, &network_number, sizeof(network_number), 0);

    printf("Número enviado: %d\n", network_number);

    close(sockfd);
    freeaddrinfo(res);
}

void send_to_manager(int final_number)
{
    struct addrinfo hints, *res;
    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int err = getaddrinfo("127.0.0.1", PORT_MANAGER, &hints, &res);
    if (err != 0)
    {
        fprintf(stderr, "getaddrinfo: %s", gai_strerror(err));
        exit(1);
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    check_error(sockfd, "socket()");

    err = connect(sockfd, res->ai_addr, res->ai_addrlen);
    check_error(err, "connect()");

    uint32_t network_number = htonl(final_number);

    ssize_t bytes_sent = send(sockfd, &network_number, sizeof(network_number), 0);

    if (bytes_sent < 0)
    {
        perror("send()");
        exit(EXIT_FAILURE);
    }

    close(sockfd);
    freeaddrinfo(res);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Deve colocar: %s <worker_number>\n", argv[0]);
        exit(1);
    }

    int worker_number = atoi(argv[1]);
    char port[5];

    calculate_port(worker_number, port);

    srand(time(NULL));

    int number = rand() % 100;

    int i = (int)log2(NUM_WORKERS);

    while (i >= 0)
    {
        if ((worker_number & (1 << i)) != 0)
        {
            server_function(&number, port);
        }
        else
        {
            client_function(&number, port);
        }

        i--;
        sleep(1);
    }

    send_to_manager(number);
    return 0;
}
