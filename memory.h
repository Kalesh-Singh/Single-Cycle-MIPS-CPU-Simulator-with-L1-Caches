#ifndef CPU_H
#define CPU_H
#include "cpu.h"
#endif

#if defined (ENABLE_L1_CACHES)
#ifndef L1_DATA_H
#define L1_DATA_H
#include "L1_data$.h"
#endif
#ifndef DM_CACHE_EXTERN
#define DM_CACHE_EXTERN
extern L1_D DM_cache;
#endif
#endif

typedef struct{
	uint32_t data_address;
	uint32_t write_data;
	uint32_t read_data;
} DATA_MEM;

typedef struct{
	uint32_t branch_address;
	uint32_t PC4_address;
	uint8_t pc_src;
	uint32_t output;
} PC_SRC_MUX;

typedef struct{
	uint32_t pc_src_mux_address;
	uint32_t jump_address;
	uint8_t jump;
	uint32_t output;
} JUMP_MUX;

uint32_t calculate_mem_address(EX_MEM_buffer* in);
int set_data_memory(EX_MEM_buffer* in, DATA_MEM* data_mem);
int set_pc_src_mux(EX_MEM_buffer* in, PC_SRC_MUX* pc_src_mux);
int set_jump_mux(EX_MEM_buffer* in, PC_SRC_MUX* pc_src_mux, JUMP_MUX* j_mux);
int mem_wb_buffer(EX_MEM_buffer *in, MEM_WB_buffer *out, DATA_MEM* data_mem);

