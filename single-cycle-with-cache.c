/**
 * Gedare Bloom
 * Kaleshwar Singh
 * Mahia Tasneem
 * Henchhing Limbu
 * Zaykha Kyaw San
 * Whitney Griffith
 * single-cycle.c
 *
 * Drives the simulation of a single-cycle processor
 */

#ifndef CPU_H
#define CPU_H
#include "cpu.h"
#endif

#ifndef MEMORY_H
#define MEMORY_H
#include "memory.h"
#endif

#ifndef L1_INSTRUCTION_H
#define L1_INSTRUCTION_H
#include "L1_instruction$.h"
#endif

#ifndef L1_DATA_H
#define L1_DATA_H
#include "L1_data$.h"
#endif

#ifndef MEMORY_STRUCTURES_H
#define MEMORY_STRUCTURES_H
#include "memory_structures.h"
#endif

#ifndef SYSCALL_H
#define SYSCALL_H
#include "syscall.h"
#endif

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDINT_H
#define STDINT_H 
#include <stdint.h>
#endif

#include <assert.h>

int main( int argc, char *argv[] )
{
//#define DEBUG
	FILE *f;
	IF_ID_buffer if_id;
	ID_EX_buffer id_ex;
	EX_MEM_buffer ex_mem;
	MEM_WB_buffer mem_wb;
	int i;

	#if defined (ENABLE_L1_CACHES)
	// Initially all valid bits are 0
	memset(IM_cache.blocks, 0, 128 * sizeof(BLOCK));
	
	memset(DM_cache.sets, 0, 128 * sizeof(BLOCK));
	#endif

	/* Initialize registers and memory to 0 */
	cpu_ctx.PC = 0x00400000;


	for ( i = 0; i < 32; i++ ) {
		cpu_ctx.GPR[i] = 0;
	}

	/* Initialize stack pointer */
	//TODO: QtSpim's stack pointer begins at 7ffff61c
	cpu_ctx.GPR[29] = 0x7FFFF61C;

	for ( i = 0; i < 1024; i++ ) {
		instruction_memory[i] = 0;
	}

	for ( i = 0; i < 8192; i++ ) {
		data_memory[i] = 0;
	}

	/* Read memory from the input file */
	f = fopen(argv[1], "r");
	assert (f);
	for ( i = 0; i < 8192; i++ ) {
		//fread(&data_memory[i], sizeof(uint8_t), 1, f);
		data_memory[i] = fgetc(f);
#if defined(DEBUG)
		printf("%u\n", data_memory[i]);
#endif
	}

	for ( i = 0; i < 1024; i++ ) {
		fread(&instruction_memory[i], sizeof(uint32_t), 1, f);
#if defined(DEBUG)
		printf("%u\n", instruction_memory[i]);
#endif
	}
	fclose(f);

#if defined(DEBUG)
	printf("\nINSTRUCTION MEMORY \n\n");
	for (i = 0; i < 30; ++i) {
		printf("%X\n", instruction_memory[i]);
	}
#endif
	while(1) {
		fetch( &if_id );
		decode( &if_id, &id_ex );
		execute( &id_ex, &ex_mem );
		memory( &ex_mem, &mem_wb );
		writeback( &mem_wb );
	}
	return 0;
}
