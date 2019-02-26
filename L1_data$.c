/*
 * Gedare Bloom
 * Kaleshwar Singh
 * Mahia Tasneem
 * Henchhing Limbu
 * Zaykha Kyaw San
 * Whitney Griffith
 * cpu.c
 *
 * Implementation of the L1 data cache.
 */

#ifndef L1_DATA_H
#define L1_DATA_H
#include "L1_data$.h"
#endif

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

int parse_address_d(uint32_t data_mem_addr, ADDRESS_PARSER_D* address_parser){
	// parse memory address to cache block components
	uint32_t temp;
	address_parser->tag_bits = data_mem_addr >> 9;
	temp = data_mem_addr << 23;
	address_parser->index_bits = (uint8_t) (temp >> 27);
	temp = data_mem_addr << 28;
	address_parser->word_offset = (uint8_t) (temp >> 30);
	temp = data_mem_addr << 28;
	address_parser->byte_offset = (uint8_t) (temp >> 28);
	return 0;
}

int set_comparator_d(ADDRESS_PARSER_D* addr_info, L1_D* cache, TAG_COMPARATOR_D* comparator, int block_index){
	// setting comparator inputs
	comparator->address_tag = addr_info->tag_bits;
	comparator->block_tag = (((cache->sets)[addr_info->index_bits]).blocks[block_index]).tag_bits; // sets[].blocks[].tag_bits
	// setting comparator output
	if (comparator->address_tag == comparator->block_tag)
		comparator->output = 1;
	else
		comparator->output = 0;
	return 0;
}

int set_and_gate_d(TAG_COMPARATOR_D* tag_comparator, L1_AND_GATE_D* and_gate, ADDRESS_PARSER_D* addr_info, L1_D* cache, int block_index){
	// set inputs for the and gate
	and_gate->tag_comparator_output = tag_comparator->output;
	and_gate->valid_bit = (((cache->sets)[addr_info->index_bits]).blocks[block_index]).valid_bit;

	// calculate and gate output
	and_gate->hit = and_gate->tag_comparator_output && and_gate->valid_bit;
	return 0;
}

// This function will be used to fetch individual tags and valid bits from each way, and then call set_comparator_d and set_and_gate_d
int set_ways(ADDRESS_PARSER_D* addr_info, L1_AND_GATE_D* way_1, L1_AND_GATE_D* way_2, L1_AND_GATE_D* way_3, L1_AND_GATE_D* way_4, L1_D* cache){
	// Temporary comparator for each way
	TAG_COMPARATOR_D temp;
	
	set_comparator_d(addr_info, cache, &temp, 0);
	set_and_gate_d(&temp, way_1, addr_info, cache, 0);
	
	set_comparator_d(addr_info, cache, &temp, 1);
	set_and_gate_d(&temp, way_2, addr_info, cache, 1);
	
	set_comparator_d(addr_info, cache, &temp, 2);
	set_and_gate_d(&temp, way_3, addr_info, cache, 2);
	
	set_comparator_d(addr_info, cache, &temp, 3);
	set_and_gate_d(&temp, way_4, addr_info, cache, 3);
	
	return 0;
} 

int set_DATA_MUX_D(L1_OR_GATE_D* or_gate, DATA_MUX_D* data_mux){
	// the or_gate will send way signal for the mux to choose what word to output
	data_mux->way = or_gate->way;

	if (data_mux->way == 1){
		data_mux->data_mem_result = data_mux->way1_word;
	}
	else
		if (data_mux->way == 2)
			data_mux->data_mem_result = data_mux->way2_word;
		else
			if (data_mux->way == 3)
				data_mux->data_mem_result = data_mux->way3_word;
			else
				if (data_mux->way == 4)
					data_mux->data_mem_result = data_mux->way4_word;
				else
					data_mux->data_mem_result = 0;
				
	return 0;
}

int set_DATA_MUX_words(ADDRESS_PARSER_D* addr_info, DATA_MUX_D* data_mux, L1_D* cache) {
	// Setting word inputs of data mux
	memcpy(&(data_mux->way1_word), &(cache->sets[addr_info->index_bits]).blocks[0].words[addr_info->word_offset], 16);	
	memcpy(&(data_mux->way2_word), &(cache->sets[addr_info->index_bits]).blocks[1].words[addr_info->word_offset], 16);
	memcpy(&(data_mux->way3_word), &(cache->sets[addr_info->index_bits]).blocks[2].words[addr_info->word_offset], 16);
	memcpy(&(data_mux->way4_word), &(cache->sets[addr_info->index_bits]).blocks[3].words[addr_info->word_offset], 16);	
	return 0;	
}

int set_L1_OR_GATE(L1_AND_GATE_D* way_1, L1_AND_GATE_D* way_2, L1_AND_GATE_D* way_3, L1_AND_GATE_D* way_4, L1_OR_GATE_D* or_gate){
	// setting L1_OR_GATE inputs
	or_gate->way1_hit = way_1->hit;
	or_gate->way2_hit = way_2->hit;
	or_gate->way3_hit = way_3->hit;
	or_gate->way4_hit = way_4->hit;
	
	// setting the outputs of L1_OR_GATE
	if (or_gate->way1_hit)
		or_gate->way = 1;
	else
		if (or_gate->way2_hit)
			or_gate->way = 2;
		else
			if (or_gate->way3_hit)
				or_gate->way = 3;
			else
				if (or_gate->way4_hit)
					or_gate->way = 4;
				else
					or_gate->way = 0;
	
	if (or_gate->way == 0)
		or_gate->hit = 0;
	else
		or_gate->hit = 1;
	return 0;
}

int update_LRU_bits(ADDRESS_PARSER_D* addr_info, L1_D* cache, uint8_t way){
	uint8_t set_no = addr_info->index_bits;
	// add 1 to LRU bits for all blocks that have lessser LRU bits than accessed or replaced block
	for (int i = 0; i < 4; ++i){
		if (cache->sets[set_no].blocks[i].LRU_bits < cache->sets[set_no].blocks[way].LRU_bits)
			cache->sets[set_no].blocks[i].LRU_bits += 1;
	}
	// set accessed or replaced block's LRU bits to 00
	cache->sets[set_no].blocks[way].LRU_bits = 0;
	return 0;
}

uint8_t select_block_replace(ADDRESS_PARSER_D* addr_info, L1_D* cache){
	uint8_t set_no = addr_info->index_bits;
	uint8_t way = 0;
	for (int i = 0; i < 4; ++i){
		if (cache->sets[set_no].blocks[i].LRU_bits > cache->sets[set_no].blocks[way].LRU_bits)
		way = i;
	}
	return way;
}

int set_evicted_block(ADDRESS_PARSER_D* addr_info, L1_D* cache, BLOCK_D* evicted_block, uint8_t way){
	// make a replica of the block from the cache being evicted
	evicted_block->dirty_bit = cache->sets[addr_info->index_bits].blocks[way].dirty_bit;
	memcpy(evicted_block->words, cache->sets[addr_info->index_bits].blocks[way].words, 16);
	evicted_block->tag_bits = cache->sets[addr_info->index_bits].blocks[way].tag_bits;
	evicted_block->valid_bit = cache->sets[addr_info->index_bits].blocks[way].valid_bit;
	evicted_block->LRU_bits = cache->sets[addr_info->index_bits].blocks[way].LRU_bits;
	
	return 0;
}
