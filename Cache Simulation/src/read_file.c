#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stddef.h>
#include <string.h>
#include "structs.h"
#include "parse.h"
#include "read_file.h"

int calculate_numRequests(FILE* file){
    char *line = NULL;
    size_t len = 0;
    int count = 0;

    while (getline(&line, &len, file) != -1) {
        count++;
    }
    free(line);
    // Reset file pointer to the beginning of the file
    fseek(file, 0, SEEK_SET); 
    return count;
}

struct Request *read_file(const char *path, size_t *numReq) {

    //check Successful file opening
    FILE *file; 
    if (!(file = fopen(path, "r"))) { 
        perror("Error opening file");
        return NULL;
    }

    //get information about the file 
    struct stat statbuffer; 
    if (fstat(fileno(file), &statbuffer)) { 
        fprintf(stderr, "Error getting the file status");
        goto cleanup;
    }

    if (!S_ISREG(statbuffer.st_mode) || statbuffer.st_size<=0) {
        fprintf(stderr, "Error processing file: Not a regular file or invalid size\n");
        goto cleanup;
    }

    //calculate length of the array 
    int numRequests=calculate_numRequests(file);
    *numReq= numRequests;

    //create requests Array 
    struct Request* requests=malloc(numRequests * sizeof(struct Request));

    //test successful memory allocation  
    if (requests == NULL) {
        fprintf(stderr, "Failed to allocate memory for requests");
        goto cleanup;
    }

    int index =0;

    // get line by line - create requests and add it to array 
    char* line=NULL;
    size_t line_size = 0;
    while ((getline(&line,&line_size,file)) != -1){
        if(parse_request(line,&requests[index++])==1){
            goto cleanup;
        }
    }

    if (ferror(file) || !feof(file)){
        goto cleanup;
    }

    // Free dynamically allocated memory
    free(line);
    
    //close file and return buffer pointer  
    if (file) fclose(file);
    return requests;

    //used to treat all errors 
    cleanup:
        if (line) free(line);
        if (file) fclose(file);
        return NULL;
}
