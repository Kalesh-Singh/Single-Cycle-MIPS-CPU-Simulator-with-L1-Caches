#ifndef CPU_H
#define CPU_H
#include "cpu.h"
#endif

typedef struct{
    uint8_t branch;
    uint8_t zero;
    uint8_t pc_src;
} AND_GATE;

typedef struct{
    uint32_t ext_immediate;
    uint32_t pc_plus_4;
	uint32_t branch_address;
} BRANCH_ADD;

typedef struct {
	uint32_t read_data2;
	uint32_t ext_immediate;
	uint32_t alu_input2;
} ALU_SRC_MUX;

typedef struct{
    uint32_t input1;
    uint32_t input2;
    uint32_t alu_result;
    uint8_t zero;
} ALU;


int alu_src(ID_EX_buffer *in, ALU_SRC_MUX *alu_src_mux);
int branch_adder(ID_EX_buffer *in, BRANCH_ADD *branch_add);
int alu_operation(ID_EX_buffer *in, ALU *alu, ALU_SRC_MUX *alu_src_mux, EX_MEM_buffer *out);
int set_and(ID_EX_buffer *in, ALU *alu, AND_GATE *and_gate);
int ex_mem_buffer(ID_EX_buffer* in, ALU *alu, BRANCH_ADD *branch_add, AND_GATE *and_gate, EX_MEM_buffer *out);
