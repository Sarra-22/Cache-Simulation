#ifndef SPEICHER_HPP
#define SPEICHER_HPP

#include <unordered_map>
#include <limits.h>

                                    // *     Abstraction of MainMemory    *//
//uses Little Endian
//gets physical adress 
//case Write -> writes in assigned Adresses 
//case Read -> reads from assigned Adresses -> gives the data back -> will be written in Requests Array 

SC_MODULE(speicher) {
    
    sc_in<bool> clk;

    sc_in<bool>run_signal;
    sc_in<uint32_t> data;     
    sc_in<uint32_t> address;  

    sc_in<bool> rw;   // Read Vs Write port 
    sc_out<uint32_t> out;  // Output port : gives Data back 


    SC_CTOR(speicher) {
        SC_THREAD (read_write_speicher);

        //read_write_speicher should be triggered by changes in run_signal
        // This means the `read_write_speicher` thread will only run when TLB changes the signal
        sensitive << run_signal; 
    }

    //mapping addresses to Data <Adress,Data>
    std::unordered_map<uint32_t, uint32_t> speicher_map;

    void read_write_speicher(){

        while(true){
            wait();

            if (rw.read()) {

                // Write operation 
                // Check if the address is within the allowed range to avoid out-of-bounds errors 
                //             -> Report error and stop simulation
                // Write Data bytes one by one

                uint32_t addr = address.read();
                uint32_t value = data.read();
                
                for (int i=0; i<4; i++) {
                     if(addr > UINT_MAX-i){
                        SC_REPORT_ERROR("TLB2","Error during write operation: Address is out of bounds." );
                        sc_stop();
                    }
                    speicher_map[addr+i] = (value >> (i * 8)) & 0xFF;
                }

            }else {

                // Perform Read Operation
                // Check if the address is within the allowed range to avoid out-of-bounds errors 
                //             -> Report error and stop simulation
                // ectract Data bytes one by one
                //recompose Data -> give it back to tlb

                uint32_t addr = address.read();
                uint32_t value_to_read = 0;

                for (int i=0; i<=3; i++) {
                    uint32_t part_of_value = 0;
                    if(addr > UINT_MAX-3+i){
                        SC_REPORT_ERROR("TLB2", "Error during Read operation: Address is out of bounds.");
                        sc_stop();
                    }
                    if (speicher_map.count(addr + 3- i)) {
                        part_of_value = speicher_map[addr+3- i];
                    }else {
                        part_of_value = 0;
                    }
                
                    value_to_read = (value_to_read << 8) | part_of_value;
                }

                out->write(value_to_read);
                wait(SC_ZERO_TIME);
            }
            wait(SC_ZERO_TIME);

        }
    }



};

#endif