/*
 * Gedare Bloom
 * Kaleshwar Singh
 * Mahia Tasneem
 * Henchhing Limbu
 * Zaykha Kyaw San
 * Whitney Griffith
 * cpu.c
 *
 * Definitions for the L1 data cache.
 */
#ifndef STDINT_H
#define STDINT_H 
#include <stdint.h>
#endif

#ifndef STRING_H
#define STRING_H
#include <string.h>
#endif

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

//#define DEBUG

// TODO: not PC but instead data mem address
typedef struct{
	uint32_t data_mem_addr;
    uint32_t tag_bits;      // 23 bits
    uint8_t index_bits;     // 5 bits
    uint8_t word_offset;    // 2 bits
	uint8_t byte_offset;	// 4 bits
}ADDRESS_PARSER_D;

typedef struct{
    uint32_t tag_bits;      // 23 bits
    uint8_t valid_bit;      // 1 bit(not from 32 bits of address)
	uint32_t words[4];		// data
	uint8_t LRU_bits;		// 2 bits
	uint8_t dirty_bit;		// 1 bit
}BLOCK_D;

typedef struct{
	BLOCK_D blocks[4];
}SET;

typedef struct{
    SET sets[32];			// 64 bytes per set
}L1_D;

// MUX to select data to be sent to the processor
typedef struct{
	uint32_t way1_word;
	uint32_t way2_word;
	uint32_t way3_word;
	uint32_t way4_word;
//	uint32_t words[4];
	uint32_t data_mem_result;
	uint8_t way;
}DATA_MUX_D;

typedef struct{
	uint32_t block_tag;
	uint32_t address_tag;
	uint8_t output;
}TAG_COMPARATOR_D;

typedef struct{
	uint8_t tag_comparator_output;
	uint8_t valid_bit;
	uint8_t hit;		// tells if the cache access is a hit or miss
}L1_AND_GATE_D;

typedef struct{
	uint8_t way1_hit;
	uint8_t way2_hit;
	uint8_t way3_hit;
	uint8_t way4_hit;
	uint8_t hit;
	uint8_t way;		// ways are basically blocks, with different tags, in the same set
}L1_OR_GATE_D;

int parse_address_d(uint32_t data_mem_addr, ADDRESS_PARSER_D* address_parser);
int set_comparator_d(ADDRESS_PARSER_D* addr_info, L1_D* cache, TAG_COMPARATOR_D* comparator, int block_index);
int set_and_gate_d(TAG_COMPARATOR_D* tag_comparator, L1_AND_GATE_D* and_gate, ADDRESS_PARSER_D* addr_info, L1_D* cache, int block_index);
int set_ways(ADDRESS_PARSER_D* addr_info, L1_AND_GATE_D* way_1, L1_AND_GATE_D* way_2, L1_AND_GATE_D* way_3, L1_AND_GATE_D* way_4, L1_D* cache);
int set_DATA_MUX_D(L1_OR_GATE_D* or_gate, DATA_MUX_D* data_mux);
int set_DATA_MUX_words(ADDRESS_PARSER_D* addr_info, DATA_MUX_D* data_mux, L1_D* cache);
int set_L1_OR_GATE(L1_AND_GATE_D* way_1, L1_AND_GATE_D* way_2, L1_AND_GATE_D* way_3, L1_AND_GATE_D* way_4, L1_OR_GATE_D* or_gate);
int update_LRU_bits(ADDRESS_PARSER_D* addr_info, L1_D* cache, uint8_t way);
uint8_t select_block_replace(ADDRESS_PARSER_D* addr_info, L1_D* cache);
int set_evicted_block(ADDRESS_PARSER_D* addr_info, L1_D* cache, BLOCK_D* evicted_block, uint8_t way);
