#ifndef READ_FILE_H
#define READ_FILE_H

#include <stdio.h>      
#include <stdlib.h>     
#include <sys/stat.h>   
#include <stddef.h>     
#include <string.h>     
#include "structs.h"    

// Function declarations
int calculate_numRequests(FILE* file);
struct Request *read_file(const char *path,size_t* req);

#endif // READ_FILE_H
