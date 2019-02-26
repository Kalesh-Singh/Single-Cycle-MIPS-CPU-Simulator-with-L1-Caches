#ifndef WRITEBACK_H
#define WRITEBACK_H
#include "writeback.h"
#endif

int set_mem_to_reg(MEM_WB_buffer* in, MEMTOREG_MUX* mem_to_reg_mux) {
    // Set the inputs of the MUX
    mem_to_reg_mux->alu_result = in->alu_result;
    mem_to_reg_mux->read_data = in->read_data;
    mem_to_reg_mux->mem_to_reg = in->mem_to_reg;

    // Set the output of MUX
    if (mem_to_reg_mux->mem_to_reg){
        mem_to_reg_mux->output = mem_to_reg_mux->read_data;
	}
    else{
        mem_to_reg_mux->output = mem_to_reg_mux->alu_result;
	}
	return 0;
}
