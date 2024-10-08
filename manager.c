#include "manager.h"


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
 * Essa main cria o servidor que printa o último resultado - o manager
 */
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
    printf("\033[35m###### MANAGER GERADO ######\033[0m\n");
    err = bind(sockfd, res->ai_addr, res->ai_addrlen);
    check_error(err, "bind()");
    err = listen(sockfd, 1);
    check_error(err, "listen()");

    int clientfd = accept(sockfd, NULL, NULL);
    check_error(clientfd, "accept()");

    char num[5]; 
    ssize_t bytes;
    bytes = recv(clientfd, num, sizeof(num), 0);
    check_error(bytes, "recv()");
    printf("\033[35mO resultado da redução é: %s\n", num);

    send(clientfd, num, sizeof(num), 0);
    close(sockfd);
    close(clientfd);
    freeaddrinfo(res);

    return 0;
}
