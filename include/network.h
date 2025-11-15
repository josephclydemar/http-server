#ifndef NETWORK_H_
#define NETWORK_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdbool.h>

int use_addr(const struct addrinfo *addr);
void show_addrinfo(const bool verbose, const struct addrinfo *info);

#endif // !NETWORK_H_

