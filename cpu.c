/*
 * Gedare Bloom
 * Kaleshwar Singh
 * Mahia Tasneem
 * Henchhing Limbu
 * Zaykha Kyaw San
 * Whitney Griffith
 * cpu.c
 *
 * Implementation of simulated processor.
 */
  
#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef CPU_H
#define CPU_H
#include "cpu.h"
#endif

#ifndef MEMORY_STRUCTURES_H
#define MEMORY_STRUCTURES_H
#include "memory_structures.h"
#endif

#ifndef DECODE_H
#define DEOCDE_H
#include "decode.h"
#endif

#ifndef EXECUTE_H
#define EXECUTE_H
#include "execute.h"
#endif

#ifndef MEMORY_H
#define MEMORY_H
#include "memory.h"
#endif

#ifndef WRITEBACK_H
#define WRITEBACK_H
#include "writeback.h"
#endif

cpu_context cpu_ctx;
PROC_stats processor_stat = {0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						 	     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
						 	     0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, 0, 0, 0, 0};


//#if defined (ENABLE_L1_CACHES)
#ifndef L1_INSTRUCTION_H
#define L1_INSTRUCTION_H
#include "L1_instruction$.h"
#endif
#ifndef IM_CACHE
#define IM_CACHE
L1_I IM_cache;
#endif
//#endif

//#define CONTROL
//#define DEBUG

int fetch( IF_ID_buffer *out )
{
#if defined (ENABLE_L1_CACHES)
	ADDRESS_PARSER addr_parser;
	DATA_MUX data_mux;
	TAG_COMPARATOR tag_comparator;
	L1_I_AND_GATE and_gate;

	// Parse the address 
	parse_address(cpu_ctx.PC, &addr_parser);
	
	// Set the data mux
	set_DATA_MUX(&addr_parser, &IM_cache, &data_mux);

	// Set the tag comparator
	set_comparator(&addr_parser, &IM_cache, &tag_comparator);

	// Set the and gate
	set_and_gate(&tag_comparator, &and_gate, &addr_parser, &IM_cache);
	
	// updating instruction access
	processor_stat.ins_cache_access +=  1;
	
	// If it is a hit
	if (and_gate.hit) {
#if defined DEBUG
	printf("Instruction Cache HIT\n");
#endif
		// Set the out buffer
		out->instruction = data_mux.instruction;
		processor_stat.ins_cache_hit += 1;

	} else { 		// If it is a miss
#if defined DEBUG
	printf("Instruction Cache MISS\n");
#endif
		
		processor_stat.ins_cache_miss += 1;
		processor_stat.stall += 4;
		processor_stat.cycle += 4;
		// Get the instruction block from memory
		uint32_t block_base_addr = cpu_ctx.PC - ((addr_parser.word_offset) * 4);
		int base_index = (block_base_addr  - 0x00400000) / 4;
		// Set the tag and valid bit
		IM_cache.blocks[addr_parser.index_bits].tag_bits = addr_parser.tag_bits;
		IM_cache.blocks[addr_parser.index_bits].valid_bit = 1;

		// Copy the words from instruction memory into the block words
		memcpy((IM_cache.blocks[addr_parser.index_bits].words), (instruction_memory + base_index), 16);

#if defined DEBUG
		printf("-----------------------------------------\n");
		printf("Valid Bit = %d\n", IM_cache.blocks[addr_parser.index_bits].valid_bit);
		printf("Tag bits = %X\n", IM_cache.blocks[addr_parser.index_bits].tag_bits);
		for (int i = 0;i < 4; ++i) {
			printf("Word %d = %X\n", i, IM_cache.blocks[addr_parser.index_bits].words[i]);
		}
		printf("-----------------------------------------\n");
#endif

		// Set the data mux
		set_DATA_MUX(&addr_parser, &IM_cache, &data_mux);
		
		// Set the out buffer
		out->instruction = data_mux.instruction;
	}
	
#endif

#ifndef ENABLE_L1_CACHES
	int index = (cpu_ctx.PC - 0x00400000) / 4;			// Convert address to  index
	out->instruction = instruction_memory[index];		// Load the instruction from IM
#endif
	
	out->next_pc = cpu_ctx.PC + 4;	 		// Increment the PC + 4										
	processor_stat.cycle += 1;
	processor_stat.useful_ins += 1;
	return 0;
}

int decode( IF_ID_buffer *in, ID_EX_buffer *out )
{
#if defined(CONTROL)
	printf("fetched instruction = %X\n", in->instruction);
#endif
	REG_DST_MUX reg_dst_mux;
	REG_FILE reg_file;
	SIGN_EXTEND sign_extend;
	CONTROL_UNIT control;
	INSTRUCTION instruction;
	
	// Parse the 32-bit instruction
	parse_instruction(in, &instruction);
	
	// Calculate the jump address
	uint32_t jump_address = calc_jump_address(in, &instruction);
	
	// Determine the control signals
	control_unit(&instruction, &control);

	// Set the reg dst must
	reg_dst(&instruction, &control, &reg_dst_mux);

	// Get the outputs from the register file
	register_file(&instruction, &reg_dst_mux, &reg_file);

	// Sign extend the immediate value
	sign_extender(&instruction, &sign_extend);

	// Set ID_EX_buffer
	id_ex_buffer(in, out, &control, &reg_file, &sign_extend, jump_address);

#if defined(CONTROL)
	printf("PC = %X\n", cpu_ctx.PC);
#endif
#if defined(DEBUG)
	printf("Opcode = %X\n", out->opcode);
	printf("Shamt = %d\n", out->shamt);
	printf("Funct = %X\n", out->funct);
	printf("mem_read = %d\n", out->mem_read);
	printf("mem_write = %d\n", out->mem_write);
	printf("mem_to_reg = %d\n", out->mem_to_reg);
	printf("alu_src = %d\n", out->alu_src);
	printf("reg_write = %d\n", out->reg_write);
	printf("branch = %d\n", out->branch);
	printf("jump = %d\n", out->jump);
	printf("jump_address = %X\n", out->jump_address);
	printf("write_reg = %d\n", out->write_reg);
	printf("read_data1 = %d\n", out->read_data1);
	printf("read_data2 = %d\n", out->read_data2);
	printf("ext_immediate = %d\n", out->ext_immediate);
#endif

	return 0;
}

int execute( ID_EX_buffer *in, EX_MEM_buffer *out )
{
    ALU alu;
    BRANCH_ADD branch_add;
    AND_GATE and_gate;
    ALU_SRC_MUX alu_src_mux;

	// Set the alu src mux
	alu_src(in, &alu_src_mux);

	// Caluculate the branch address
	branch_adder(in, &branch_add);	

	// Perform the alu operation
	alu_operation(in, &alu, &alu_src_mux, out);

	// Get the pc src signal
	set_and(in, &alu, &and_gate);

    // set EX_MEM_buffer
    ex_mem_buffer(in, &alu, &branch_add, &and_gate, out);
#if defined DEBUG
	printf("ALU INPUT 1 = %d\n", alu.input1);
	printf("ALU INPUT 2 = %d\n", alu.input2);
	printf("ALU ZERO = %d\n", alu.zero);
#endif

    return 0;
}

int memory( EX_MEM_buffer *in, MEM_WB_buffer *out )
{
	DATA_MEM data_mem;
	PC_SRC_MUX pc_src_mux;
	JUMP_MUX j_mux;

	// Read or write to data/stack memory 
	set_data_memory(in, &data_mem);

	// Set the pc src mux
	set_pc_src_mux(in, &pc_src_mux);

#if defined DEBUG
	if (pc_src_mux.pc_src)
		printf("BRANCH TAKEN\n");
	else
		printf("BRANCH NOT TAKEN\n");
#endif

	// Set the jump mux
	set_jump_mux(in, &pc_src_mux, &j_mux);

	// Set the next address of the PC the output of Jump MUX
	cpu_ctx.PC = j_mux.output;

	mem_wb_buffer(in, out, &data_mem);

	return 0;
}

int writeback( MEM_WB_buffer *in)
{
    MEMTOREG_MUX mem_to_reg_mux;

	// Set the mem to reg mux
	set_mem_to_reg(in, &mem_to_reg_mux);

    // Write to the register file if reg wrtie is asserted
    if (in->reg_write) {
#if defined (DEBUG)
	printf("Performing writeback..\n");
#endif
        cpu_ctx.GPR[in->write_reg] = mem_to_reg_mux.output;
#if defined (DEBUG)
	printf("Writeback complete..\n");
#endif
	}
	
#if defined (DEBUG)
	printf("alu result for mux = %X\n", mem_to_reg_mux.alu_result);
	printf("read data for mux = %X\n", mem_to_reg_mux.read_data);
	printf("mem_to_reg mux output = %X\n", mem_to_reg_mux.output);
	printf("$zero or $0 = %d\n", cpu_ctx.GPR[0]);
	printf("$at or $1 = %X\n", cpu_ctx.GPR[1]);
	printf("Data to be written: %d\n", mem_to_reg_mux.output);
	printf("____________________________________________________________________\n\n");
#endif

	return 0;
}
