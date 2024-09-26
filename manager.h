#ifndef MANAGER_H
#define MANAGER_H
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
void check_error(const int code, const char *func_name);

#endif