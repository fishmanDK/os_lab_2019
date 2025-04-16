#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include "multmodulo.h"

struct Server {
    char ip[255];
    int port;
};

bool ConvertStringToUI64(const char *str, uint64_t *val) {
    char *end = NULL;
    unsigned long long i = strtoull(str, &end, 10);
    if (errno == ERANGE) {
        fprintf(stderr, "Out of uint64_t range: %s\n", str);
        return false;
    }
    *val = i;
    return true;
}

void parseServers(struct Server* to, unsigned int* servers_num, const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Unable to open file");
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%254[^:]:%d", to[*servers_num].ip, &to[*servers_num].port);
        (*servers_num)++;
    }
    fclose(file);
}

int main(int argc, char **argv) {
    uint64_t k = 0;
    uint64_t mod = 0;
    char servers[255] = {'\0'};

    while (1) {
        static struct option options[] = {
            {"k", required_argument, 0, 0},
            {"mod", required_argument, 0, 0},
            {"servers", required_argument, 0, 0},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "", options, &option_index);
        if (c == -1) break;

        switch (c) {
        case 0:
            if (option_index == 0) ConvertStringToUI64(optarg, &k);
            else if (option_index == 1) ConvertStringToUI64(optarg, &mod);
            else if (option_index == 2) strncpy(servers, optarg, sizeof(servers)-1);
            break;
        }
    }

    if (!k || !mod || !strlen(servers)) {
        fprintf(stderr, "Usage: %s --k <value> --mod <value> --servers <file>\n", argv[0]);
        exit(1);
    }

    unsigned int servers_num = 0;
    struct Server *to = malloc(sizeof(struct Server) * 10);
    parseServers(to, &servers_num, servers);

    uint64_t total_result = 1;
    for (int i = 0; i < servers_num; i++) {
        struct hostent *hostname = gethostbyname(to[i].ip);
        if (!hostname) {
            fprintf(stderr, "gethostbyname failed for %s\n", to[i].ip);
            exit(1);
        }

        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(to[i].port);
        memcpy(&server.sin_addr.s_addr, hostname->h_addr_list[0], hostname->h_length);

        int sck = socket(AF_INET, SOCK_STREAM, 0);
        if (sck < 0) {
            fprintf(stderr, "Socket creation error\n");
            exit(1);
        }

        if (connect(sck, (struct sockaddr*)&server, sizeof(server)) < 0) {
            fprintf(stderr, "Connection failed to %s:%d\n", to[i].ip, to[i].port);
            exit(1);
        }

        uint64_t task[3];
        task[0] = 1 + i*(k/servers_num);
        task[1] = (i == servers_num-1) ? k : (i+1)*(k/servers_num);
        task[2] = mod;

        send(sck, task, sizeof(task), 0);
        
        uint64_t server_result;
        recv(sck, &server_result, sizeof(server_result), 0);
        total_result = MultModulo(total_result, server_result, mod);
        
        close(sck);
    }

    free(to);
    printf("Final result: %" PRIu64 "\n", total_result);
    return 0;
}