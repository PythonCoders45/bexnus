#include "net.h"
#include "rtl8139.h"
#include "../drivers/vga.h"

void net_init(void) {
    puts("net: init\n");
    rtl8139_init();
}

int net_send_raw(const uint8_t *data, int len) {
    return rtl8139_send(data, len);
}

int net_poll_raw(uint8_t *buf, int max_len) {
    return rtl8139_poll(buf, max_len);
}
