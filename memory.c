#ifndef MEMORY_H
#define MEMORY_H
#include "memory.h"
#endif

#ifndef MEMORY_STRUCTURES_H
#define MEMORY_STRUCTURES_H
#include "memory_structures.h"
#endif

//#if defined (ENABLE_L1_CACHES)
#ifndef L1_DATA_H
#define L1_DATA_H
#include "L1_data$.h"
#endif
#ifndef DM_CACHE
#define DM_CACHE
L1_D DM_cache;
#endif
//#endif


#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif


uint32_t calculate_mem_address(EX_MEM_buffer* in) {
	// Returns the read/write address of data memory or stack memory
	// Accessing data memory
	if ((in->alu_result >= 0x10010000) && (in->alu_result < 0x1003FFFF)) 			
		return in->alu_result - 0x10010000;
	// Accessing stack memory
	else if ((in->alu_result > 0x10010000) && (in->alu_result <= 0x7FFFF61C))		
		return  8192 - (0x7FFFF61C - in->alu_result);
	// Error
	return -1;		
}

int set_data_memory(EX_MEM_buffer* in, DATA_MEM* data_mem) {
	data_mem->data_address = calculate_mem_address(in);
	data_mem->write_data = in->read_data2;
#if defined (ENABLE_L1_CACHES)
	if (in->mem_read || in->mem_write){
		ADDRESS_PARSER_D addr_parser;
		DATA_MUX_D data_mux;
		TAG_COMPARATOR_D tag_comparator;
		L1_AND_GATE_D way_1, way_2, way_3, way_4;
		L1_OR_GATE_D or_gate;
		BLOCK_D evicted_block;		// to check if dirty block 

		// updating the access
		processor_stat.data_cache_access += 1;
		
		// Parse the address 
		parse_address_d(data_mem->data_address, &addr_parser);

		// Set the tag comparator and AND gates for each way
		set_ways(&addr_parser, &way_1, &way_2, &way_3, &way_4, &DM_cache);

		// Set the OR gate
		set_L1_OR_GATE(&way_1, &way_2, &way_3, &way_4, &or_gate);
		
		// Set data mux words
		set_DATA_MUX_words(&addr_parser, &data_mux, &DM_cache);
		
		// Set the data mux
		set_DATA_MUX_D(&or_gate, &data_mux);
		
		#if defined DEBUG
		printf("Tag bits = %X\n", addr_parser.tag_bits);
		printf("Index bits = %X\n", addr_parser.index_bits);
		printf("Word offset = %X\n", addr_parser.word_offset);
		printf("Byte offset = %X\n", addr_parser.byte_offset);
		#endif
				
		// If it is a hit
		if (or_gate.hit) {
		#if defined DEBUG
			if (in->mem_read){
				printf("--------------------------------\n");
				printf("Data Cache Read HIT\n");
				printf("-------------------------------\n");
			}
		#endif
			// TODO: Fix this
			processor_stat.data_cache_hit += 1;
			uint32_t temp = 0;
			uint32_t temp_word = 0;
			// Set the data mem read data
			data_mem->read_data = data_mux.data_mem_result;
			// New shifting endian
			for (int i = 0; i < 4 ; i++){
				temp_word = temp_word << 8;
				temp = data_mem->read_data << (24 - (8 * i));
				temp = temp >> 24;
				temp_word = temp_word | temp;
			}
			
			data_mem->read_data = temp_word;
			// Update LRU bits
			uint8_t way = (or_gate.way - 1);
			update_LRU_bits(&addr_parser, &DM_cache, way);
			
			// Updating cache block if mem_write
			if(in->mem_write) {
				#if defined DEBUG
					printf("--------------------------------\n");
					printf("Data Cache Write HIT\n");
					printf("-------------------------------\n");
				#endif
				// Update dirty bit to 1
				DM_cache.sets[addr_parser.index_bits].blocks[way].dirty_bit = 1;	
				
				// Writing to cache block (x86 -> MIPS) i.e Little Endian to Big Endian
				temp_word = 0;
				temp = 0;
				for (int i = 0; i < 4 ; i++){
					temp_word = temp_word << 8;
					temp = data_mem->write_data << (24 - (8 * i));
					temp = temp >> 24;
					temp_word = temp_word | temp;
				}
				DM_cache.sets[addr_parser.index_bits].blocks[way].words[addr_parser.word_offset] = temp_word;
			}
		} 
		// If it is a miss
		else { 		
		#if defined DEBUG
		if (in->mem_read){
			printf("--------------------------------\n");
			printf("Data Cache Read MISS\n");
			printf("-------------------------------\n");
		}
		#endif
			// TODO: Fix this
			processor_stat.data_cache_miss += 1;
			processor_stat.stall += 4;
			processor_stat.cycle += 4;
			uint32_t temp_word = 0;

			// Get the data block from memory
			uint32_t base_index = (data_mem->data_address) - (addr_parser.byte_offset);

			// Set the way and update the LRU bits before replacing the block
			uint8_t way = select_block_replace(&addr_parser, &DM_cache);
			set_evicted_block(&addr_parser, &DM_cache, &evicted_block, way);
			update_LRU_bits(&addr_parser, &DM_cache, way);
			
			// Set the tag and valid bit
			DM_cache.sets[addr_parser.index_bits].blocks[way].tag_bits = addr_parser.tag_bits;
			DM_cache.sets[addr_parser.index_bits].blocks[way].valid_bit = 1;
			
			// Copy the words from data memory into the block words
			for (int i = 0; i < 4; ++i){
				for (int j = 0; j < 4; ++j){
					temp_word = temp_word << 8;
					temp_word = temp_word | data_memory[base_index + j];
				}
				DM_cache.sets[addr_parser.index_bits].blocks[way].words[i] = temp_word;
				base_index += 4;
				temp_word = 0;
			}
			// Set the dirty bit to 0 for new block
			DM_cache.sets[addr_parser.index_bits].blocks[way].dirty_bit  = 0;
			
			uint32_t temp;
			if (in->mem_write){
				// In case of a write miss, copy the cache block to data memory
				#if defined DEBUG
					printf("--------------------------------\n");
					printf("Data Cache Write MISS\n");
					printf("-------------------------------\n");
				#endif
				// Update dirty bit of new block to 1
				DM_cache.sets[addr_parser.index_bits].blocks[way].dirty_bit = 1;
				// Writing to cache block (x86 -> MIPS) i.e Little Endian to Big Endian
				uint32_t temp_word = 0;
				for (int i = 0; i < 4 ; i++){
					temp_word = temp_word << 8;
					temp = data_mem->write_data << (24 - (8 * i));
					temp = temp >> 24;
					temp_word = temp_word | temp;
				}
				DM_cache.sets[addr_parser.index_bits].blocks[way].words[addr_parser.word_offset] = temp_word;
			}
			// Copy evicted block to data memory if its dirty bit is 1
			if (evicted_block.dirty_bit == 1){
				uint32_t temp_index = evicted_block.tag_bits;
				temp_index = temp_index << 5;
				temp_index = temp_index | addr_parser.index_bits;
				temp_index = temp_index << 4;
				memcpy((data_memory + temp_index), evicted_block.words , 16);
			}
			// Set the out buffer
			data_mem->data_address = calculate_mem_address(in);
			
			if (in->mem_read){
				data_mem->read_data = DM_cache.sets[addr_parser.index_bits].blocks[way].words[addr_parser.word_offset];
			}
		}
	}
#endif

#ifndef ENABLE_L1_CACHES
	uint32_t temp;
	int i;

	// Writing to data/stack memory (x86 -> MIPS i.e Little Endian to Big Endian)
	if (in->mem_write) {
		for (i = 0; i < 4; i++) {
			temp = data_mem->write_data << (8 * i);
			data_memory[data_mem->data_address + (3 - i)] = temp >> 24;
		}
	}

	// Reading from data/stack memory (MIPS -> x86 i.e Big Endian to Little Endian)
	if (in->mem_read) {
		temp = 0;
		for ( i = 3; i >= 0; --i ) {
			temp = temp << 8;
			temp = temp | data_memory[data_mem->data_address + i];
		}
		data_mem->read_data = temp;
	}
#endif
	return 0;
}

int set_pc_src_mux(EX_MEM_buffer* in, PC_SRC_MUX* pc_src_mux) {
	// Set the inputs of PC MUX
	pc_src_mux->pc_src = in->pc_src;
	pc_src_mux->PC4_address = in->next_pc;
	pc_src_mux->branch_address = in->branch_address;

	// Set the output of PC MUX
	if (pc_src_mux->pc_src)
		pc_src_mux->output = pc_src_mux->branch_address;
	else
		pc_src_mux->output = pc_src_mux->PC4_address;

	return 0;
}

int set_jump_mux(EX_MEM_buffer* in, PC_SRC_MUX* pc_src_mux, JUMP_MUX* j_mux) {

	// Set the inputs of jump MUX
	j_mux->pc_src_mux_address = pc_src_mux->output;
	j_mux->jump_address = in->jump_address;
	j_mux->jump = in->jump;

	// Set the output of jump MUX
	if (j_mux->jump)
		j_mux->output = j_mux->jump_address;
	else
		j_mux->output = j_mux->pc_src_mux_address;

	return 0;
}

int mem_wb_buffer(EX_MEM_buffer *in, MEM_WB_buffer *out, DATA_MEM* data_mem) {

	// Passing data to MEM/WB Buffer
	out->alu_result = in->alu_result;
	out->mem_to_reg = in->mem_to_reg;
	out->reg_write = in->reg_write;
	out->write_reg = in->write_reg;
	out->read_data = data_mem->read_data;

	return 0;
}



