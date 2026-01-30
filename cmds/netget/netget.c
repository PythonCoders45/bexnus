#include <stdint.h>
#include "../kernel/drivers/vga.h"
#include "../kernel/drivers/http.h"

void cmd_netget(int argc, char **argv) {
    if (argc < 2) {
        puts("usage: netget <url>\n");
        return;
    }

    const char *url = argv[1];

    uint8_t buf[4096];
    int n = http_get(url, buf, sizeof(buf));

    if (n <= 0) {
        puts("netget: failed\n");
        return;
    }

    puts("=== RESPONSE BEGIN ===\n");
    for (int i = 0; i < n; i++)
        putc(buf[i]);
    puts("\n=== RESPONSE END ===\n");
}
