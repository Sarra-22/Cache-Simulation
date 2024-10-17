#ifndef PARSE_H
#define PARSE_H

#include "structs.h"
#include "stdbool.h"

int parse_long(const char *s, unsigned *d);
int parse_decimal_32uinteger(const char *s, unsigned *d);
int parse_hex_32uinteger(const char *s, unsigned *d);
int parse_request(char* line, struct Request* address);
bool zweierpotenz (unsigned n);

#endif // PARSE_H
