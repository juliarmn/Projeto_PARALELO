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
#define CHAR_NUM_SIZE 4

/**
 * @param const int code: o que foi devolvido ao chamar a função
 * @param const char* func_name: representa o nome da função para a mensagem de erro correta
 *
 * Se for -1 tem algo errado, então mostra a mensagem de erro e encerra o codigo.
 */
void check_error(const int code, const char *func_name)
{
    if (code == -1)
    {
        perror(func_name);

        exit(1);
    }
}

/**
 * @param int numero enviado no terminal
 * @param char* porta correspondente - modifica o ponteiro
 *
 * Salva a porta correta de acordo com o worker
 */
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
        fprintf(stderr, "\033[31mNúmero de worker inválido (deve ser entre 0 e 7).\n");
        exit(1);
        break;
    }
}

/**
 * @param char número recebido: aquele que o cliente enviou para o cálculo
 * @param char* port: porta do servidor a se conectar
 *
 * Cria um servidor que recebe o número e calcula o necessário.
 *
 */
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
        fprintf(stderr, "\033[31mgetaddrinfo: %s", gai_strerror(err));
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

    char num_to_receive[CHAR_NUM_SIZE];
    ssize_t bytes = recv(clientfd, num_to_receive, sizeof(num_to_receive), 0);
    check_error(bytes, "recv()");

    int num_to_sum_server = atoi(num);
    int num_to_sum_client = atoi(num_to_receive);

    num_to_sum_server += num_to_sum_client;

    sprintf(num, "%d", num_to_sum_server);

    printf("\033[34mNúmero somado pelo servidor: %s\n", num);
    printf("\033[34m/-------------------------/\n");

    send(clientfd, num, sizeof(num), 0);
    close(sockfd);
    close(clientfd);
    freeaddrinfo(res);
}

/**
 * @param char* num: número do cliente - que ele envia para o servidor
 * @param char* port
 *
 * Função cliente: cria um socket cliente que envia o número para o servidor e recebe sua confiamção.
 *
 */
void *client_function(char *num, char *PORT)
{
    struct addrinfo hints, *res;
    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int err = getaddrinfo("127.0.0.1", PORT, &hints, &res);

    if (err != 0)
    {
        fprintf(stderr, "\033[31mgetaddrinfo: %s", gai_strerror(err));
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
            perror("\033[32mconnect");
            printf("\033[32mTentando conectar em 1 segundo...\n");
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

    printf("\033[35mNúmero somado recebido pelo cliente: %s\n", num);
    printf("\033[35m/-------------------------/\n");
    close(sockfd);
    freeaddrinfo(res);
}

/**
 * @param char* final_number
 *
 * Recebe o número final e envia para o manager, encerrando o processo.
 * Ou seja, cria um cliente no final.
 */
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
            perror("\033[32mconnect");
            printf("\033[32mTentando conectar em 1 segundo no último...\n");
            sleep(1);
        }
    }

    ssize_t bytes_sent = send(sockfd, final_number, sizeof(final_number), 0);

    if (bytes_sent < 0)
    {
        perror("send()");
        exit(EXIT_FAILURE);
    }
    ssize_t bytes;

    bytes = recv(sockfd, final_number, sizeof(final_number), 0);
    check_error(bytes, "recv()");
    close(sockfd);
    freeaddrinfo(res);
}

/**
 * @param int argc: quantas palavras/ parametros foram lidos ao compilar
 * @param char* argv para pegar o número do worker
 *
 * Calcula a porta e faz a lógica do borboleta.
 * Na lógica usa bitwise, com count múltiplo de 2 para saber se os bits correspondentes a cada camada são 1 ou 0.
 * Assim, define qual  worker é cliente e qual é servidor.
 */
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "\033[31mDeve colocar: %s <worker_number>\n", argv[0]);
        exit(1);
    }

    int worker_number = atoi(argv[1]);
    char port[5];

    calculate_port(worker_number, port);

    srand(time(NULL));

    int number = rand() % 100;
    char number_to_send[CHAR_NUM_SIZE];
    sprintf(number_to_send, "%d", number);

    printf("\033[33mNo %d gerou numero %d\033[0m\n", worker_number, number);

    int i = (int)log2(NUM_WORKERS) - 1;
    int count = 1;
    int controller = 0;
    int neighboor;

    while (i >= 0)
    {
        if ((worker_number & count) != 0)
        {
            printf("\033[35m/-------------------------/\n");
            printf("\033[35mCliente %d enviou numero: %s \n", worker_number, number_to_send);
            neighboor = worker_number ^ count;
            char port_neighboor[5];
            calculate_port(neighboor, port);
            client_function(number_to_send, port_neighboor);
            break;
        }
        else
        {
            printf("\033[34m/-------------------------/\n");
            printf("\033[34mServidor %d enviou numero: %s \n", worker_number, number_to_send);
            server_function(number_to_send, port);
        }

        count = count * 2;
        i--;
    }

    if (worker_number == 0)
    {
        send_to_manager(number_to_send);
    }

    return 0;
}
