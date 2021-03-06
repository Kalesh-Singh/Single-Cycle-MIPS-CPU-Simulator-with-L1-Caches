/**
 * Gedare Bloom
 * Kaleshwar Singh
 * Mahia Tasneem
 * Henchhing Limbu
 * Zaykha Kyaw San
 * Whitney Griffith
 * cpu.h
 *
 * Definitions for the processor.
 */

// Enable / Disable IM Cache
#define ENABLE_L1_CACHES

#ifndef STDINT_H
#define STDINT_H 
#include <stdint.h>
#endif

#ifndef STRING_H
#define STRING_H 
#include <string.h>
#endif

typedef struct {
	uint32_t PC;
	uint32_t GPR[32];
} cpu_context;

extern cpu_context cpu_ctx;

#if defined (ENABLE_L1_CACHES)
#ifndef L1_INSTRUCTION_H
#define L1_INSTRUCTION_H
#include "L1_instruction$.h"
#endif
#ifndef IM_CACHE_EXTERN
#define IM_CACHE_EXTERN
extern L1_I IM_cache;
#endif
#endif

typedef struct {
    uint16_t add;
    uint16_t addu;
    uint16_t addi;
    uint16_t and_ctr;
    uint16_t andi;
    uint16_t beq;
    uint16_t bne;
    uint16_t j;
    uint16_t jal;
    uint16_t jr;
    uint16_t lui;
    uint16_t lw;
    uint16_t nor;
    uint16_t or_ctr;
    uint16_t ori;
    uint16_t slt;
    uint16_t sll;
    uint16_t srl;
    uint16_t slti;
    uint16_t sub;
    uint16_t sw;
    uint16_t sra;
    uint16_t xor_ctr;
    uint16_t xori;
    uint16_t syscall;
    uint16_t unimp_r_type;
    uint16_t unimp_i_type;
    uint16_t nop;
} INSTR_ctr;

typedef struct {
    uint16_t cycle;
    INSTR_ctr instruction;
	uint16_t ins_cache_miss;
	uint16_t ins_cache_hit;
	uint16_t data_cache_miss;
	uint16_t data_cache_hit;
	uint16_t ins_cache_access;
	uint16_t data_cache_access;
	uint16_t stall;
	uint16_t useful_ins;
} PROC_stats;

extern PROC_stats processor_stat;

typedef struct {
	uint32_t instruction;
	uint32_t next_pc;
} IF_ID_buffer;

typedef struct{
	uint32_t next_pc;
	uint8_t opcode;			// Execute
	uint8_t shamt;			// Execute
	uint8_t funct;			// Execute
	uint8_t mem_read;
	uint8_t mem_write;
	uint8_t mem_to_reg;
	uint8_t alu_src;		// Execute
	uint8_t reg_write;
	uint8_t branch;			// Execute
	uint8_t jump;
	uint32_t jump_address;
	uint8_t write_reg;
	uint32_t read_data1;	// Execute
	uint32_t read_data2;	// Execute
	uint32_t ext_immediate;	// Execute
} ID_EX_buffer;

typedef struct{
	uint32_t next_pc;
    uint8_t mem_read;
    uint8_t mem_write;
    uint8_t mem_to_reg;
    uint8_t reg_write;
    uint8_t jump;
    uint8_t write_reg;
    uint32_t read_data2;
    uint32_t jump_address;
    uint32_t alu_result;
    uint8_t pc_src;
    uint32_t branch_address;        // From the branch adder
    // Note PC+4 can be accessed by cpu_ctx.PC
} EX_MEM_buffer;

typedef struct{
	uint32_t read_data;
	uint32_t alu_result;
	uint8_t mem_to_reg;
	uint8_t reg_write;
    uint8_t write_reg;
} MEM_WB_buffer;

int fetch( IF_ID_buffer *out );
int decode( IF_ID_buffer *in, ID_EX_buffer *out );
int execute( ID_EX_buffer *in, EX_MEM_buffer *out );
int memory( EX_MEM_buffer *in, MEM_WB_buffer *out );
int writeback( MEM_WB_buffer *in );
