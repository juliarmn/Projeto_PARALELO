#include "worker.h"

/*Struct para controlar o envio dos dados como número e estágio*/
 struct worker
{
    char work_number[CHAR_NUM_SIZE];
    int stage;
    int accepted;
    int server_value;
} ;

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
 * @param char* porta que vai salvar o valor.
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
 * @param Worker struct que possui os dados do worker
 * @param char* porta a se conectar
 * 
 * Cria o servidor, que valida se os processos estão no mesmo estágio, caso estejam em estágios diferentes ele encerra a conexão.
 * Só aceita conexão de processos em estágios iguais.
 * Soma o valor e envia ele de volta.
 */
void server_function(Worker *worker, char *PORT)
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

    int opt = 1;
    err = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    check_error(err, "setsockopt()");

    err = bind(sockfd, res->ai_addr, res->ai_addrlen);
    check_error(err, "bind()");
    err = listen(sockfd, 8);
    check_error(err, "listen()");

    int clientfd = accept(sockfd, NULL, NULL);
    check_error(clientfd, "accept()");

    Worker worker_to_receive;
    ssize_t bytes = recv(clientfd, &worker_to_receive, sizeof(Worker), 0);
    check_error(bytes, "recv()");

    if (worker_to_receive.stage != worker->stage)
    {
        printf("Processos em estágios diferentes, aguarde...\n");
        worker->server_value = 0;
        worker_to_receive.accepted = 0;
        send(clientfd, &worker_to_receive, sizeof(Worker), 0);
    }
    else
    {
        worker->accepted = 1;
        worker->server_value = 1;
        int num_to_sum_server = atoi(worker->work_number);
        int num_to_sum_client = atoi(worker_to_receive.work_number);

        num_to_sum_server += num_to_sum_client;

        sprintf(worker->work_number, "%d", num_to_sum_server);

        printf("\033[34mNúmero somado pelo servidor: %s\n", worker->work_number);
        printf("\033[34m/-------------------------/\n");
        send(clientfd, worker, sizeof(Worker), 0);
    }

    close(clientfd);
    close(sockfd);
    freeaddrinfo(res);
}

/**
 * @param Worker *wroker que irá enviar seu número
 * @param char* PORT porta a ser enviada
 * 
 * Cria o cliente e envia o valor para  o servidor (caso seja aceito termina corretamente, senão tenta no futuro conectar até estarem no mesmo estágio)
 * 
 */
void client_function(Worker *worker, char *PORT)
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
            printf("\033[32mTentando conectar em 1 segundo...\n");
            sleep(1);
        }
    }

    ssize_t bytes_sent = send(sockfd, worker, sizeof(Worker), 0);

    if (bytes_sent < 0)
    {
        perror("send()");
        exit(EXIT_FAILURE);
    }
    ssize_t bytes;

    bytes = recv(sockfd, worker, sizeof(Worker), 0);

    if (worker->accepted == 0)
    {
        close(sockfd);
        freeaddrinfo(res);
    }
    else
    {
        printf("\033[35mNúmero somado recebido pelo cliente: %s\n", worker->work_number);
        printf("\033[35m/-------------------------/\n");
        close(sockfd);
        freeaddrinfo(res);
    }
}

/**
 * @param int numero final da soma
 * Envia o número final ao manager
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
 * @param int argc - quantidade de parâmetros no terminal
 * @param char* argv[] - é aquele que manda o número do worker
 * 
 * Aqui implementa a borboleta, fazendo bitwise and a cada estágio, descobre quem é cliente e quem é servidor.
 * Com bitwise xor ele descobre o vizinho naquela rodada.
 * Faz a verificação de o cliente e server estavam no  mesmo estágio, senão mantém o cliente tentando conectar.
 */
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "\033[31mDeve colocar: %s <worker_number>\n", argv[0]);
        exit(1);
    }

    int worker_number = atoi(argv[1]);
    Worker worker;
    worker.accepted = 0;
    worker.stage = 0;
    char port[5];

    calculate_port(worker_number, port);

    srand(time(NULL));

    int number = rand() % 100;
    char number_to_send[CHAR_NUM_SIZE];
    sprintf(number_to_send, "%d", number);
    strcpy(worker.work_number, number_to_send);
    printf("\033[33mNo %d gerou numero %d\033[0m\n", worker_number, number);

    int i = (int)log2(NUM_WORKERS) - 1;
    int count = 1;
    int neighboor;
    int cliente = 0;

    while (i >= 0)
    {
        if ((worker_number & count) != 0)
        {
            cliente = 1;
            printf("\033[35m/-------------------------/\n");
            printf("\033[35mCliente %d enviou numero: %s \n", worker_number, worker.work_number);
            neighboor = worker_number ^ count;
            char port_neighboor[5];
            calculate_port(neighboor, port_neighboor);
            client_function(&worker, port_neighboor);

            while (worker.accepted == 0)
            {
                printf("Cliente no aguardo...\n");
                sleep(1);
                client_function(&worker, port_neighboor);
            }

            break;
        }
        else
        {
            worker.server_value = 1;
            printf("\033[34m/-------------------------/\n");
            printf("\033[34mServidor %d enviou numero: %s \n", worker_number, worker.work_number);
            server_function(&worker, port);
            while (!worker.server_value)
                server_function(&worker, port);
            worker.stage++;
        }

        count = count * 2;
        i--;
    }

    if (worker_number == 0)
    {
        send_to_manager(worker.work_number);
    }

    return 0;
}
