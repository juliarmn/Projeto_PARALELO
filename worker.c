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

#define PORT_WORKER_0 "8081"
#define PORT_WORKER_1 "8082"
#define PORT_WORKER_2 "8083"
#define PORT_WORKER_3 "8084"
#define PORT_WORKER_4 "8085"
#define PORT_WORKER_5 "8086"
#define PORT_WORKER_6 "8087"
#define PORT_WORKER_7 "8088"
#define PORT_MANAGER "8080"
#define NUM_WORKERS 8

void check_error(const int code, const char *func_name)
{
    if (code == -1)
    {
        perror(func_name);

        exit(1);
    }
}

void calculate_port(int num, char *port)
{
    switch (num)
    {
    case 0:
        strcpy(port, PORT_WORKER_0);
        break;
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
    default:
        fprintf(stderr, "Número de worker inválido (deve ser entre 0 e 7).\n");
        exit(1);
        break;
    }
}

void *server_function(char *num, char *PORT)
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
    check_error(clientfd, "accept()");

    char num_to_receive[4];
    ssize_t bytes = recv(clientfd, num_to_receive, sizeof(num_to_receive), 0);
    check_error(bytes, "recv()");

    int num_to_sum_server = atoi(num);
    int num_to_sum_client = atoi(num_to_receive);

    num_to_sum_server += num_to_sum_client;

    sprintf(num, "%d", num_to_sum_server);

    printf("Número somado pelo servidor: %s\n", num);

    send(clientfd, num, strlen(num), 0);
    close(sockfd);
    close(clientfd);
    freeaddrinfo(res);
}

void *client_function(char *num, char *PORT)
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

        while (1)
    {
        err = connect(sockfd, res->ai_addr, res->ai_addrlen);
        if (err != -1)
        {
            break;
        }
        else
        {
            perror("connect");
            printf("Retrying connection in 1 second...\n");
            sleep(1);
        }
    }


    ssize_t bytes_sent = send(sockfd, num, sizeof(num), 0);

    if (bytes_sent < 0)
    {
        perror("send()");
        exit(EXIT_FAILURE);
    }
    ssize_t bytes;

    bytes = recv(sockfd, num, sizeof(num), 0);

    printf("Número somado recebido pelo cliente: %s\n", num);
    printf("/-------------------------/\n");
    close(sockfd);
    freeaddrinfo(res);
}

void send_to_manager(char *final_number)
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

        while (1)
    {
        err = connect(sockfd, res->ai_addr, res->ai_addrlen);
        if (err != -1)
        {
            break;
        }
        else
        {
            perror("connect");
            printf("Retrying connection in 1 second...\n");
            sleep(1);
        }
    }


    ssize_t bytes_sent = send(sockfd, final_number, sizeof(final_number), 0);

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
    char number_to_send[4];
    sprintf(number_to_send, "%d", number);

    printf("No %d gerou numero %d\n", worker_number, number);

    int i = (int)log2(NUM_WORKERS) - 1;

    while (i >= 0)
    {
        if (i == (int)log2(NUM_WORKERS) - 1 || (i == (int)log2(NUM_WORKERS) - 2 && worker_number % 2 == 0) || i == 0 && (worker_number == 0 || worker_number == 4))
        {
            if (worker_number & (1 << i) != 0)
            {
                printf("/-------------------------/\n");
                printf("Cliente %d enviou numero: %s \n", worker_number, number_to_send);
                if (worker_number == 1 || worker_number == 2 || worker_number == 4)
                {
                    client_function(number_to_send, PORT_WORKER_0);
                }
                else if (worker_number == 3)
                {
                    client_function(number_to_send, PORT_WORKER_2);
                }
                else if (worker_number == 5 || worker_number == 6)
                {
                    client_function(number_to_send, PORT_WORKER_4);
                }
                else if (worker_number == 7)
                {
                    client_function(number_to_send, PORT_WORKER_6);
                }
            }
            else
            {
                printf("Servidor %d enviou numero: %s \n", worker_number, number_to_send);
                server_function(number_to_send, port);
            }
        }
        i--;
    }

    send_to_manager(number_to_send);
    return 0;
}
