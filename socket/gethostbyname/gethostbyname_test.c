#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
        return 1;
    }

    const char *hostname = argv[1];

    struct hostent *host = gethostbyname(hostname);
    if (host == NULL) {
        fprintf(stderr, "Failed to get hostent for hostname: %s\n", hostname);
        return 1;
    }

    printf("Hostname: %s\n", host->h_name);

    printf("Aliases:\n");
    char **alias = host->h_aliases;
    while (*alias != NULL) {
        printf("  %s\n", *alias);
        alias++;
    }

    printf("IP Addresses:\n");
    char **addr = host->h_addr_list;
    while (*addr != NULL) {
        char ip[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, *addr, ip, INET_ADDRSTRLEN) != NULL) {
            printf("  %s\n", ip);
        }
        addr++;
    }

    return 0;
}