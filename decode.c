#ifndef DECODE_H
#define DEOCDE_H
#include "decode.h"
#endif

// Decode Stage Helper Functions
int parse_instruction(IF_ID_buffer *in, INSTRUCTION *instruction){
    instruction->opcode = (in->instruction >> 26);	// Opcode
    uint32_t temp = (in->instruction) << 6;
    instruction->rs = temp >> 27;					// rs
    temp = (in->instruction) << 11;
    instruction->rt = temp >> 27;					// rt
    temp = (in->instruction) << 16;
    instruction->rd = temp >> 27;					// rd
    temp = (in->instruction) << 16;
    instruction->immediate = temp >> 16;			// immediate
    temp = (in->instruction) << 21;
    instruction->shamt = temp >> 27;				// shamt
    temp = (in->instruction) << 26;
    instruction->funct = temp >> 26;				// funct
	temp = (in->instruction) << 6;
	instruction->jump_26lower = temp >> 6;			// lower 28 bits of the jump address
	// No need to shift left 2, because our memory is instruction addressable not byte addressable
	return 0;
}

uint32_t calc_jump_address(IF_ID_buffer *in, INSTRUCTION *instruction){
	// Returns the calculated jump address
	uint32_t pc_high_4 = in->next_pc >> 28;
	pc_high_4 = pc_high_4 << 28;						// upper 4 bits of the jump address
	uint32_t jump_28lower = instruction->jump_26lower << 2;
	uint32_t jump_address = pc_high_4 | jump_28lower;	// jump_address
	// TODO: Test this
	// jump_address = jump_address - (0x00400000 / 4);	
	return jump_address;
}

int control_unit(INSTRUCTION *instruction, CONTROL_UNIT *control){
	control->opcode = instruction->opcode;
	control->shamt = instruction->shamt;
	control->funct = instruction->funct;
	
	control->mem_read = 0;
	control->mem_to_reg = 0;
	control->mem_write = 0;
	control->alu_src = 0;
	control->reg_write = 0;
	control->reg_dst = 0;
	control->branch = 0;
	control->jump = 0;

	switch (control->opcode) {		
		case 0x00:		// R-Format
			control->reg_dst = 1;		
			// if not nop, jr, or syscall, only then assert reg_write 
			if (!((control->funct == 0 && instruction->rd == 0) 
				|| control->funct == 0x08 || control->funct == 0x0C))		
				control->reg_write = 1;																						
			break;
		case 0x23:		// lw
			control->alu_src = 1;
			control->mem_to_reg = 1;
			control->reg_write = 1;
			control->mem_read = 1;
			break;

		case 0x2B:		// sw
			control->alu_src = 1;
			control->mem_write = 1;
			break;

		case 0x04:		// beq
		case 0x05:		// bne
			control->branch = 1;
			break;

		case 0x02:		// j
		case 0x03:		// jal
			control->jump = 1;
			break;

		case 0x08:		// addi
		case 0x0C:		// andi
		case 0x0F:		// lui
		case 0x0D:		// ori
		case 0x0A:		// slti
		case 0x0E:		// xori
			control->alu_src = 1;
			control->reg_write = 1;
			break;
		default:
			break;
	}
	return 0;
}

int reg_dst(INSTRUCTION *instruction, CONTROL_UNIT *control, REG_DST_MUX *reg_dst_mux){
	// Set the input of mux
	reg_dst_mux->rt = instruction->rt;
	reg_dst_mux->rd = instruction->rd;
	if (control->reg_dst)
		reg_dst_mux->write_reg = reg_dst_mux->rd;
	else
		reg_dst_mux->write_reg = reg_dst_mux->rt;
	return 0;
}

int register_file(INSTRUCTION *instruction, REG_DST_MUX *reg_dst_mux, REG_FILE *reg_file){
	reg_file->read_reg1 = instruction->rs;
	reg_file->read_reg2 = instruction->rt;
	
	// Set the write reg input of the register file
	reg_file->write_reg = reg_dst_mux->write_reg;

	// Set the outputs of the register file
	reg_file->read_data1 = cpu_ctx.GPR[reg_file->read_reg1];
	reg_file->read_data2 = cpu_ctx.GPR[reg_file->read_reg2];

	return 0;
}

int sign_extender(INSTRUCTION *instruction, SIGN_EXTEND *sign_extend){
	// Set the outputs of sign extend
	uint8_t sign = (instruction->immediate) >> 15;		// The sign of the immediate
	sign_extend->ext_immediate = instruction->immediate;
	if (sign) {
		sign_extend->ext_immediate = sign_extend->ext_immediate | 0xFFFF0000;
	}
	return 0;
}

int id_ex_buffer(IF_ID_buffer *in, ID_EX_buffer *out, CONTROL_UNIT *control, REG_FILE *reg_file, SIGN_EXTEND *sign_extend, uint32_t jump_address){
	out->next_pc = in->next_pc;
	out->opcode = control->opcode;
	out->shamt = control->shamt;
	out->funct = control->funct;
	out->mem_read = control->mem_read;
	out->mem_write = control->mem_write;
	out->mem_to_reg = control->mem_to_reg;
	out->alu_src = control->alu_src;
	out->reg_write = control->reg_write;
	out->branch = control->branch;
	out->jump = control->jump;
	out->jump_address = jump_address;
	out->write_reg = reg_file->write_reg;
	out->read_data1 = reg_file->read_data1;
	out->read_data2 = reg_file->read_data2;
	out->ext_immediate = sign_extend->ext_immediate;

	return 0;
}
// ENd of decode stage helper functions

