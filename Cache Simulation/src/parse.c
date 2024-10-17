#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "structs.h"
#include "stdbool.h"

const char* format_msg = 
        "Error: Invalid CSV line format: %s\n"
        "The valid format for write is: W, address, data.\n"
        "The valid format for read is: R, address.\n" ;

int parse_long(const char *s, unsigned *d) {
    errno = 0;
    char* endptr;
    unsigned long l = strtoul(s, &endptr,10);
    
    if (endptr == s || *endptr != '\0' || l> UINT32_MAX) {
        fprintf(stderr, "Invalid number: %s could not be converted \n", s);
        return 1;
    } else if (errno == ERANGE) {
        fprintf(stderr, "Invalid number: %s over- or underflows \n", s);
        return 1;
    }
    *d=l;
    return 0;
}

int parse_decimal_32uinteger(const char *s, unsigned *d) {
    errno = 0; 
    char *end;
    unsigned long result = strtoul(s, &end,10);
    if (end == s || *end != '\0' || errno==ERANGE ||  result > UINT32_MAX ) {//*d > UINT_MAX
        fprintf(stderr,"Invalid number: ");
        return 1;
    }
    *d=result;
    return 0;
}

int parse_hex_32uinteger(const char *s, unsigned *d) {
    errno = 0;
    char *end;
    unsigned long result = strtoul(s, &end,16);
    if (end == s || *end != '\0' || errno==ERANGE || result > UINT32_MAX ) {
        fprintf(stderr,"Invalid number: ");
        return 1;
    }
    *d=result;
    return 0;
}

int parse_request(char* line, struct Request* adress){
    struct Request request;
    char *token;
    // Tokenize the line by commas
    line[strcspn(line, "\r\n")] ='\0';
    token = strtok(line, ",");
    if (!token) {
       goto error_clean;
    }

    // First token: type (assuming first element is 'R' or 'W')
    if (strcmp(token, "W") == 0) { 
        request.we = 1;
    } else if (strcmp(token, "R") == 0) {
        request.we = 0;
    } else {
        goto error_clean;
    }

    // Second token: address
    token = strtok(NULL, ",");
    if (!token) {
       goto error_clean;
    }
    if (strstr(token, "0x") == token) {
        if (parse_hex_32uinteger(token, &request.addr) !=0) {
            goto error_clean;   
        }   
     } else {
        if (parse_decimal_32uinteger(token, &request.addr) != 0) {
            goto error_clean;   
        }    
    }
    
    // Third token: data (if it exists)
    token = strtok(NULL, ",");
    if (token) {
        if(request.we==1){
            if (strstr(token, "0x") == token) {
                if (parse_hex_32uinteger(token, &request.data) !=0) {
                    goto error_clean;   
                }   
            } else {
                if (parse_decimal_32uinteger(token, &request.data) != 0) {
                    goto error_clean;   
                }
            }
        }else {
            goto error_clean;   
        }
    } else {
       if(request.we==0){
            request.data = 0;
        }else{
            goto error_clean;
        }
    }

    //store request in the array
    *adress=request;
    return 0;

    error_clean:
        fprintf(stderr, format_msg,line);
        return 1;
}
bool zweierpotenz (unsigned n) {
    if (n == 0) {
        return false;
    }
    return  n> 0 && (n & (n-1))==0;
}