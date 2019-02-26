/*
 * Gedare Bloom
 * Kaleshwar Singh
 * Mahia Tasneem
 * Henchhing Limbu
 * Zaykha Kyaw San
 * Whitney Griffith
 * cpu.c
 *
 * Definitions for the L1 instruction cache.
 */
#ifndef STDINT_H
#define STDINT_H 
#include <stdint.h>
#endif

#ifndef STRING_H
#define STRING_H
#include <string.h>
#endif

typedef struct{
	uint32_t PC;
    uint32_t tag_bits;      // 21 bits
    uint8_t index_bits;     // 7 bits
    uint8_t word_offset;    // 2 bits
}ADDRESS_PARSER;

typedef struct{
    uint32_t tag_bits;      // 24 bits
    uint8_t valid_bit;      // 1 bit(not from 32 bits of address)
	uint32_t words[4];		// data
}BLOCK;

typedef struct{
    BLOCK blocks[128];
}L1_I;

// MUX to select data to be sent to the processor
typedef struct{
	uint32_t words[4];
	uint8_t word_offset; 	// signal to the mux?
	uint32_t instruction;
}DATA_MUX;

typedef struct{
	uint32_t block_tag;
	uint32_t address_tag;
	uint8_t output;
}TAG_COMPARATOR;

typedef struct{
	uint8_t tag_comparator_output;
	uint8_t valid_bit;
	uint8_t hit;		// tells if the cache access is a hit or miss
}L1_I_AND_GATE;

int parse_address(uint32_t PC, ADDRESS_PARSER* address_parser);
int set_DATA_MUX(ADDRESS_PARSER* input, L1_I* cache, DATA_MUX* output);
int set_comparator(ADDRESS_PARSER* addr_info, L1_I* cache, TAG_COMPARATOR* comparator);
int set_and_gate(TAG_COMPARATOR* tag_comparator, L1_I_AND_GATE* and_gate, ADDRESS_PARSER* addr_info, L1_I* cache);
