#include <systemc>
#include <systemc.h>
using namespace sc_core;
#include "structs.h"
#include "TLB2.hpp"
#include <stdexcept> 
#include <iostream>
#include "speicher.hpp"
#include <limits.h>



struct Result run_simulation (
    int cycles, 
    unsigned tlbSize,
    unsigned tlbsLatency,
    unsigned blocksize,
    unsigned v2bBlockOffset,
    unsigned memoryLatency,
    int numRequests,
    Request requests[], 
    const char * tracefile
) {
    
    sc_clock clk("clk", 1, SC_SEC);
    
    // setting tlb signals
    sc_signal<int> hits_sig;
    sc_signal<bool> finished_sig;
    sc_signal<int> misses_sig;    

    // binding tlb to signals 

    TLB2 tlb_module("tlb_module");
    tlb_module.clk.bind(clk);
    tlb_module.hits.bind(hits_sig);
    tlb_module.finished.bind(finished_sig);
    tlb_module.misses.bind(misses_sig);

    // tlb variables initialising 

    tlb_module.tlbSize = tlbSize;
    tlb_module.v2bBlockOffset = v2bBlockOffset;
    tlb_module.blocksize = blocksize;
    tlb_module.tlbsLatency = tlbsLatency;
    tlb_module.memoryLatency = memoryLatency;
    tlb_module.requests=requests;
    tlb_module.numRequests=numRequests;

    tlb_module.updateParameters();

    // initialise MainMmeory 

    speicher speicher("speicher");

// *     Binding Tlb to MainMemory    * //

    speicher.data.bind(tlb_module.data_signal);
    speicher.rw.bind(tlb_module.rw_signal);
    speicher.address.bind(tlb_module.address_signal);
    speicher.out.bind(tlb_module.data_out_signal);
    speicher.clk.bind(clk);
    speicher.run_signal.bind(tlb_module.run_speicher);

    sc_trace_file* trace_file=NULL;

    if (tracefile){

        trace_file=sc_create_vcd_trace_file(tracefile);
        sc_trace(trace_file,hits_sig,"hits_number");
        sc_trace(trace_file,misses_sig,"misses_number");

    }

    try {
        sc_start(cycles, SC_SEC); // Run the simulation for 'cycles' seconds
    }
    catch (const std::exception& e) { //simulation stopped due to Adress range overflow
        std::cerr << "Simulation error: " << e.what() << std::endl;
        if (trace_file) sc_close_vcd_trace_file(trace_file);
        return {UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX} ; // Return error result
    }

    sc_time simulation_duration = sc_time_stamp(); // Record end time

    size_t hits = hits_sig.read();
    size_t misses = misses_sig.read();
    size_t primitiveGateCount =tlbSize*blocksize*32+93+31*150+182+64*8+93+32*4+182;

    // tlbSize*blocksize*32 for the speicher of the data of the tlb
    //93 for the shift
    //31*150 for the multiplizierer
    // 182 for the addierer and the multiplizierer
    //64*8+93+32*4+182 for the logic of the mod and div 2 Blocks of 64 bits and 1 block of 32 bits and 
    // one shift and one sub/add

    size_t number_of_cycles;
    if (finished_sig.read()) {
        number_of_cycles = simulation_duration.to_seconds();
    } else {
        number_of_cycles = SIZE_MAX;
    }
    
    if (tracefile){
        sc_close_vcd_trace_file(trace_file);
    }
    
    return { number_of_cycles, misses, hits, primitiveGateCount };
}

//  we need this default sc_main implementation.
int sc_main(int argc, char* argv[])
{
    std::cout << "ERROR" << std::endl;
    return 1;
}