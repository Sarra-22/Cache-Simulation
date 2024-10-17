#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <sys/stat.h> 
#include <stddef.h> 
#include <stdint.h> 
#include "read_file.h"
#include "parse.h"
#include "msg.h"
# include "stdbool.h"
#include "structs.h"
#include <limits.h>

//linking C to C++
extern struct Result run_simulation(
    int cycles, 
    unsigned tlbSize,
    unsigned tlbsLatency,
    unsigned blocksize,
    unsigned v2bBlockOffset,
    unsigned memoryLatency,
    int numRequests,
    struct Request* requests,
    const char * tracefile
);

void print_usage(const char* progname) {
    fprintf(stderr, usage_msg, progname);
}

void print_help(const char* progname) {
    fprintf(stderr, help_msg, progname);
}


int main(int argc, char** argv) {

    const char* progname = argv[0];
    if (argc == 1) {
            print_usage(progname);
            return EXIT_FAILURE;
    }
    
//   initialise standard values 

    int opt;
    int cycles=1000;
    unsigned tlb_size=16;
    unsigned tlb_latency=1;
    unsigned blocksize=32;
    unsigned v2b_block_offset=1;
    unsigned memory_latency=50;
    const char *tracefile = NULL;
    size_t numRequests=0;

// *  Parsing  * //
    
    struct option long_options[] = {
            {"cycles", required_argument, NULL, 'c'},
            {"blocksize", required_argument, NULL, 'b'},
            {"v2b-block-offset", required_argument, NULL, 'v'},
            {"tlb-size", required_argument, NULL, 't'},
            {"tlb-latency", required_argument, NULL, 'l'},
            {"memory-latency", required_argument, NULL, 'm'},
            {"tf", required_argument, NULL, 'f'},
            {"help", no_argument, NULL, 'h'},
            {NULL, 0, NULL, 0}
        };

    while ((opt = getopt_long(argc, argv, "c:h", long_options, NULL)) != -1){

        switch(opt){

            case 'c':
                if(parse_long(optarg,&cycles)==1){
                    fprintf(stderr, "Error: Invalid cycles  \n");
                    print_usage(progname);
                    return EXIT_FAILURE;
                }
                break;

            case 'h':
                print_help(progname);
                return EXIT_FAILURE;
                break;

            case 'b':
                if(parse_long(optarg,&blocksize)==1){
                    fprintf(stderr, "Error: Invalid Block size \n ");
                    print_usage(progname);
                    return EXIT_FAILURE;
                }
                if (!zweierpotenz(blocksize)) {
                    fprintf(stderr, "Error: Invalid Block size \n This Programm support only powers of two Blocksize ");
                    print_usage(progname);
                    return EXIT_FAILURE; 
                }
                
            break;
            case 'v' :
                if(parse_long(optarg,&v2b_block_offset)==1){
                    fprintf(stderr, "Error: Invalid v2b_block_offset \n ");
                    print_usage(progname);
                    return EXIT_FAILURE;
                }
                break;
            
            case 't' : 
                if(parse_long(optarg,&tlb_size)==1 ){
                    fprintf(stderr, "Error: Invalid tlb_size \n ");
                    print_usage(progname);
                    return EXIT_FAILURE;
                }
                break;

            case 'l' : 
                if(parse_long(optarg,&tlb_latency)==1){
                    fprintf(stderr, "Error: Invalid tlb_latency \n ");
                    print_usage(progname);
                    return EXIT_FAILURE;
                }
                if(tlb_latency<1 || tlb_latency>10){
                    fprintf(stderr, "Error: Invalid TLB latency \n TlbLatency should be in [1,10] \n");
                    print_usage(progname);
                    return EXIT_FAILURE;
                }
                break;

            case 'm' : 
                if(parse_long(optarg,&memory_latency)==1)
                {
                    print_usage(progname);
                return EXIT_FAILURE;
                }
                if(memory_latency<10 || memory_latency>100)
                {
                    fprintf(stderr, "Error: Invalid Memory latency \n");
                    return EXIT_FAILURE;
                }
            break;
            
            case 'f' : 
                tracefile = optarg;
                break;

            default:
                print_usage(progname);
                return EXIT_FAILURE;
            
        }

    }
    if(tlb_latency >= memory_latency){
        fprintf(stderr, "Logic error: Tlb_latency have to be smaller than Memory_latency");
        print_usage(progname);
        return EXIT_FAILURE;
    }

// Test Positional Argument 

    if(optind != argc -1){
            print_usage(progname);
            return EXIT_FAILURE;
    }

    const char* file_name= argv[optind];

// make sure ".csv" file 

    if (strcmp(&file_name[strlen(file_name) - 4], ".csv") != 0) {
        print_usage(progname);
        return EXIT_FAILURE;
    }

//  Create Requests Array

    struct Request* requests=read_file(file_name,&numRequests);

    if (requests == NULL) {
        fprintf(stderr, "Failed to read file '%s'\n", file_name);
        if(requests)free(requests);
        return 1;
    }

//Start simulation 
    
    struct Result result = run_simulation(
        cycles,
        tlb_size,
        tlb_latency,
        blocksize,
        v2b_block_offset,
        memory_latency,
        numRequests,
        requests,
        tracefile);


//Test Simulation successful Vs failed 
    
    if (result.cycles==UINT_MAX && result.hits==UINT_MAX && result.misses==UINT_MAX && result.primitiveGateCount==UINT_MAX)
    {
        fprintf(stderr, "Error: Simulation failed.\n");
        free(requests);
        return 1;
    }
    

    printf("Cycles: %zu\n", result.cycles);
    printf("Misses: %zu\n", result.misses);
    printf("Hits: %zu\n", result.hits);
    printf("Primitive Gate Count: %zu\n", result.primitiveGateCount);
    free(requests);
    return 0;

}
