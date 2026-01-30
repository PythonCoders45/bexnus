#ifndef BEXNUS_HTTP_H
#define BEXNUS_HTTP_H

#include <stdint.h>

int http_get(const char *url, uint8_t *out_buf, int max_len);

#endif
