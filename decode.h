#ifndef CPU_H
#define CPU_H
#include "cpu.h"
#endif


typedef struct {
	uint8_t opcode;
	uint8_t shamt;
	uint8_t funct;
	uint8_t mem_read;
	uint8_t mem_to_reg;
	uint8_t mem_write;
	uint8_t alu_src;
	uint8_t reg_write;
	uint8_t reg_dst;
	uint8_t branch;
	uint8_t jump;
} CONTROL_UNIT;

typedef struct {
	uint8_t read_reg1;
	uint8_t read_reg2;
	uint8_t write_reg;
	uint8_t write_data;
	uint32_t read_data1;
	uint32_t read_data2;
} REG_FILE;

typedef struct {
	uint16_t immediate;
	uint32_t ext_immediate;
} SIGN_EXTEND;

typedef struct {
	uint8_t rt;
	uint8_t rd;
	uint8_t write_reg;
} REG_DST_MUX;
// end of decode stage

typedef struct {
	uint8_t opcode;
	uint8_t rs;
	uint8_t rt;
	uint8_t rd;
	uint8_t shamt;
	uint8_t funct;
	uint16_t immediate;
	uint32_t jump_26lower;
} INSTRUCTION;

// Decode stage helper functions
int parse_instruction(IF_ID_buffer *in, INSTRUCTION *instruction);
uint32_t calc_jump_address(IF_ID_buffer *in, INSTRUCTION *instruction);
int control_unit(INSTRUCTION *instruction, CONTROL_UNIT *control);
int reg_dst(INSTRUCTION *instruction, CONTROL_UNIT *control, REG_DST_MUX *reg_dst_mux);
int register_file(INSTRUCTION *instruction, REG_DST_MUX *reg_dst_mux, REG_FILE *reg_file);
int sign_extender(INSTRUCTION *instruction, SIGN_EXTEND *sign_extend);
int id_ex_buffer(IF_ID_buffer *in, ID_EX_buffer *out, CONTROL_UNIT *control, REG_FILE *reg_file, SIGN_EXTEND *sign_extend, uint32_t jump_address);
