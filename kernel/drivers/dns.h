#ifndef BEXNUS_DNS_H
#define BEXNUS_DNS_H

#include <stdint.h>

int dns_resolve(const char *name, uint32_t *out_ip);

#endif
