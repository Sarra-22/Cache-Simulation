#ifndef TLB2_HPP
#define TLB2_HPP
#include <systemc>
#include <systemc.h>
using namespace sc_core;

#include <cstdint>
#include <unordered_map>
#include "structs.h"
#include <stdbool.h>
#include <cmath>

//link C++ to C 
extern "C" struct Result run_simulation(
    int cycles, 
    unsigned tlbSize,
    unsigned tlbsLatency,
    unsigned blocksize,
    unsigned v2bBlockOffset,
    unsigned memoryLatency,
    int numRequests,
    Request* requests,
    const char * tracefile);
    

SC_MODULE(TLB2) {    

// initialise Ports and variables
     sc_out<int> hits; 
    sc_out<int> misses;
    sc_in<bool> clk;

 //finished : signal indicating all requests processed successfully before #cycles 
    sc_out<bool> finished; 

    unsigned tlbSize;
    unsigned blocksize;
    unsigned v2bBlockOffset;
    unsigned tlbsLatency;
    unsigned memoryLatency ;

// track uint32 Overflows when Reading/Writing in Memory
    uint32_t maxAdress; 

// optim : used for optimisation, indicates #bits in offset_block  
    uint32_t optim;  

// b : used to get physical_block
    uint32_t b ;   

    Request* requests;
    int numRequests;
    int i=0;

//Connections with MainMemory 
    sc_signal<uint32_t> data_out_signal;
    sc_signal<bool> rw_signal;
    sc_signal<uint32_t> address_signal;
    sc_signal<uint32_t> data_signal;
    sc_signal<bool> run_speicher;

    // TLB map: set_number -> TLB entry
    std::unordered_map<unsigned, TLBEntry> tlbMap;
    
    SC_CTOR(TLB2){
        SC_THREAD(process_request);
        sensitive << clk.pos();
    }

    void updateParameters(){

        maxAdress=UINT_MAX-v2bBlockOffset*blocksize;

        // Goal: Adjust v2bBlockOffset to the closest power of 2
        int i =0;
        int bprime=v2bBlockOffset;
        while(bprime%2==0){
            i++;
            bprime=bprime/2;
        }
        b=bprime;
        optim=i+log2(blocksize); //Blocksize is already power of 2
    }

    uint32_t calculate_physical_address(uint32_t virt_block) {
        return virt_block + b;
    }

    //SC_METHOD(process_method);
    void process_request() {
        while(true)
        {
            finished.write(false);  
            wait();

            // * All requests processed * //
            if (i == numRequests) { 
                finished.write(true);
                wait(SC_ZERO_TIME);
                sc_stop();
                break;
            }

            Request* request = requests+i;

            uint32_t virtual_address = request->addr;

    // check Overflows before accessing Memory 
    // stop simulation in case of Accessing non accessible memoryadresses
            if(virtual_address>maxAdress){
                SC_REPORT_ERROR("TLB2",  "Calculated physical address exceeds maximum allowed address ");
                sc_stop();
            }
    // Adress decomposition in blocks virtual_adress = virt_block|Offset_block  and  virt_block = Tag|Set_Index 

            uint32_t virt_block=virtual_address >> optim;
            unsigned Set_index = virt_block % tlbSize;

            //get the only optim first bits from virt_adress
            uint32_t offset_block= virtual_address & (uint32_t)((std::pow(2,optim))-1);
            
            //checking if this virtual_block is already in tlb
            if (tlbMap.count(Set_index)) {
                
                TLBEntry& entry = tlbMap[Set_index];

                //same Set & same vir_blocks -> Hit -> adress translated -> No calculation
                if (entry.virt_block == virt_block) { 

                    wait(tlbsLatency); 
                    hits->write(hits.read()+1);

                }else {
                //same Set & different vir_blocks -> Miss -> calculate new phys_block + overwrite 
                    wait(memoryLatency);

                    entry.virt_block=virt_block;
                    entry.physical_block=calculate_physical_address(virt_block);
                    misses->write(misses.read()+1);

                }
             }
             else{
                // Empty Set -> initialise Entry + Miss
                wait(memoryLatency);
                TLBEntry new_value;
                new_value.virt_block=virt_block;
                new_value.physical_block=calculate_physical_address(virt_block);
                tlbMap[Set_index] = new_value;
                misses->write(misses.read()+1);

            }
        //recompose physical adress = physical_block|offset_block
            address_signal.write((tlbMap[Set_index].physical_block << optim )+ offset_block);  
           
           // write signals to ports in Memory -> get Data 
            if (request->we) {
                data_signal.write(request->data);
            }
            rw_signal.write(request->we); 
            run_speicher.write(!run_speicher.read());

            //wait for signals to be written 
            wait(SC_ZERO_TIME);

            if (request->we==0) { 
                
                wait(SC_ZERO_TIME);
                request->data=data_out_signal.read();
                
            }

            i++;
        }

    }

};

#endif