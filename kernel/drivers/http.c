#include "http.h"
#include "dns.h"
#include "tcp.h"
#include "../drivers/vga.h"
#include <string.h>

// parse "http://host/path"
static int parse_url(const char *url, char *host, char *path) {
    if (strncmp(url, "http://", 7) == 0)
        url += 7;

    const char *slash = strchr(url, '/');
    if (!slash) {
        strcpy(host, url);
        strcpy(path, "/");
        return 0;
    }

    int host_len = slash - url;
    memcpy(host, url, host_len);
    host[host_len] = 0;

    strcpy(path, slash);
    return 0;
}

int http_get(const char *url, uint8_t *out_buf, int max_len) {
    char host[128];
    char path[256];

    parse_url(url, host, path);

    puts("http: resolving host...\n");
    uint32_t ip;
    if (dns_resolve(host, &ip) != 0) {
        puts("http: DNS failed\n");
        return -1;
    }

    puts("http: connecting...\n");
    uint16_t src_port;
    uint32_t seq, ack;

    if (tcp_connect(ip, 80, &src_port, &seq, &ack) != 0) {
        puts("http: TCP connect failed\n");
        return -1;
    }

    puts("http: sending GET...\n");

    char req[512];
    int req_len = snprintf(req, sizeof(req),
        "GET %s HTTP/1.0\r\n"
        "Host: %s\r\n"
        "User-Agent: BexnusOS\r\n"
        "Connection: close\r\n\r\n",
        path, host
    );

    tcp_send(ip, src_port, 80, &seq, ack, (uint8_t*)req, req_len);

    puts("http: waiting for response...\n");

    // crude wait loop
    for (int i = 0; i < 2000000; i++) {
        extern void eth_poll(void);
        eth_poll();

        int n = tcp_recv(out_buf, max_len);
        if (n > 0)
            return n;
    }

    puts("http: timeout\n");
    return -1;
}
