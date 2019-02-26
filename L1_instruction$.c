/*
 * Gedare Bloom
 * Kaleshwar Singh
 * Mahia Tasneem
 * Henchhing Limbu
 * Zaykha Kyaw San
 * Whitney Griffith
 * cpu.c
 *
 * Implementation of the L1 instruction cache.
 */

#ifndef L1_INSTRUCTION_H
#define L1_INSTRUCTION_H
#include "L1_instruction$.h"
#endif

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif


//#define DEBUG

int parse_address(uint32_t PC, ADDRESS_PARSER* address_parser){
	// parse memory address to cache block components
	uint32_t temp;
	address_parser->tag_bits = PC >> 11;
	temp = PC << 21;
	address_parser->index_bits = (uint8_t) (temp >> 25);
	temp = PC << 28;
	address_parser->word_offset = (uint8_t) (temp >> 30);
	return 0;
}

int set_and_gate(TAG_COMPARATOR* tag_comparator, L1_I_AND_GATE* and_gate, ADDRESS_PARSER* addr_info, L1_I* cache){
	// set inputs for the and gate
	and_gate->tag_comparator_output = tag_comparator->output;
	and_gate->valid_bit = cache->blocks[addr_info->index_bits].valid_bit;

	// calculate and gate output
	and_gate->hit = and_gate->tag_comparator_output && and_gate->valid_bit;

#if defined DEBUG
	printf("------------------------------------------------\n");
	printf("Comparator Output = %d\n", and_gate->tag_comparator_output);
	printf("Valid Bit = %d\n", and_gate->valid_bit);
	for (int i = 0;i < 4; ++i) {
		printf("Word %d = %X\n", i, cache->blocks[addr_info->index_bits].words[i]);
	}
	printf("------------------------------------------------\n");
#endif

	return 0;
}

int set_DATA_MUX(ADDRESS_PARSER* input, L1_I* cache, DATA_MUX* output) {
	output->word_offset = input->word_offset;
	// copying words[4] from cache to the instruction mux words[4]
	memcpy(output->words, (cache->blocks[input->index_bits]).words, 16);
	// putting data from the block to the instruciton mux data	
	output->instruction = output->words[input->word_offset];	
	return 0;
}   

int set_comparator(ADDRESS_PARSER* addr_info, L1_I* cache, TAG_COMPARATOR* comparator) {
	// setting comparator inputs
	comparator->address_tag = addr_info->tag_bits;
	comparator->block_tag = ((cache->blocks)[addr_info->index_bits]).tag_bits;
#if defined DEBUG
	printf("------------------------------------------------\n");
	printf("Address Tag = %X\n", comparator->address_tag);
	printf("Block Tag = %X\n", comparator->block_tag);
	printf("------------------------------------------------\n");
#endif	
	// setting comparator output
	if (comparator->address_tag == comparator->block_tag)
		comparator->output = 1;
	else
		comparator->output = 0;
	return 0;
}
