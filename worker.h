#ifndef WORKER_H
#define WORKER_H
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

typedef struct worker Worker;

void check_error(const int code, const char *func_name);
void calculate_port(int num, char *port);
void server_function(Worker *worker, char *PORT);
void client_function(Worker *worker, char *PORT);
void send_to_manager(char *final_number);


#endif