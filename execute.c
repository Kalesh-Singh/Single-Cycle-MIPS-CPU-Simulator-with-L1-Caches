#ifndef EXECUTE_H
#define EXECUTE_H
#include "execute.h"
#endif

#ifndef SYSCALL_H
#define SYSCALL_H
#include "syscall.h"
#endif

// Start of execute stage helper functions
int alu_src(ID_EX_buffer *in, ALU_SRC_MUX *alu_src_mux){
	#if defined (DEBUG)
		if (in->mem_read || in->mem_write) {
			printf("The base address is = %d\n", in->read_data1);
		}
	#endif
	// set mux inputs
    alu_src_mux->read_data2 = in->read_data2;
    alu_src_mux->ext_immediate = in->ext_immediate;

    // calculate mux output
    if (in->alu_src)
        alu_src_mux->alu_input2 = alu_src_mux->ext_immediate;
    else
        alu_src_mux->alu_input2 = alu_src_mux->read_data2;

	return 0;
}

int branch_adder(ID_EX_buffer *in, BRANCH_ADD *branch_add){
	// set branch adder input
    branch_add->ext_immediate = in->ext_immediate << 2;
    branch_add->pc_plus_4 = in->next_pc;
	
	// calculate the branch address
	branch_add->branch_address = branch_add->ext_immediate + branch_add->pc_plus_4 - 4;	
    // branch_add->branch_address = branch_add->ext_immediate + branch_add->pc_plus_4 - 1;	// TODO: In QtSpim branch is calculated using PC not PC + 4, hence -1 fixes the problem

	return 0;
}

int alu_operation(ID_EX_buffer *in, ALU *alu, ALU_SRC_MUX *alu_src_mux, EX_MEM_buffer *out){

    // set alu input
    alu->input1 = in->read_data1;
    alu->input2 = alu_src_mux->alu_input2;

	out->next_pc = in->next_pc;		// Must be set here to accommodate the case that the instruction is jr

#if defined(CONTROL)
	printf("INSTRUCTION: ");	
#endif 
    switch (in->opcode)
    {
        case 0x00:
        // R-format
            switch (in->funct)
            {
				case 0x21:
		    		// addu
					alu->alu_result = alu->input1 + alu->input2;
            		#if defined(CONTROL)
            			printf("addu\n");
            		#endif
            		processor_stat.instruction.addu += 1;
		    		break;
                case 0x20:
                    // add
                    alu->alu_result = alu->input1 + alu->input2;
				#if defined(CONTROL)
					printf("add\n");
				#endif
					processor_stat.instruction.add += 1;
                    break;
                case 0x24:
                    // and
                    alu->alu_result = alu->input1 & alu->input2;
				#if defined(CONTROL)
					printf("and\n");
				#endif
					processor_stat.instruction.and_ctr += 1;
                    break;
                case 0x08:
                    // jr
                    // jumps to the address in $rs or input1 of the ALU
                    out->next_pc = alu->input1;
				#if defined(CONTROL)
					printf("jr\n");	
				#endif
                    processor_stat.instruction.jr += 1;
                    break;
                case 0x27:
                    // nor
                    alu->alu_result = ~(alu->input1 | alu->input2);
				#if defined(CONTROL)
					printf("nor\n");
				#endif
					processor_stat.instruction.nor += 1;
                    break;
                case 0x25:
                    // or
                    alu->alu_result = alu->input1 | alu->input2;
				#if defined(CONTROL)
					printf("or\n");
				#endif
					processor_stat.instruction.or_ctr += 1;
                    break;
                case 0x2A:
                    // slt
                    alu->alu_result = (alu->input1 < alu->input2) ? 1 : 0;
				#if defined(CONTROL)
					printf("slt\n");
				#endif
					processor_stat.instruction.slt += 1;
                    break;
                case 0x00:
					switch (in->write_reg) {
						case 0x00:
							// nop
						#if defined(CONTROL)
							printf("nop\n");
						#endif
						processor_stat.instruction.nop += 1;
							break;
						default:
                    		// sll
                    		alu->alu_result = alu->input2 << in->shamt;
						#if defined(CONTROL)
							printf("sll\n");
						#endif
						processor_stat.instruction.sll += 1;
                    		break;
					}
                    break;
                case 0x02:
                    // srl
                    alu->alu_result = alu->input2 >> in->shamt;
				#if defined(CONTROL)
					printf("srl\n");
				#endif
					processor_stat.instruction.srl += 1;
                    break;
                case 0x22:
                    // sub
                    alu->alu_result = alu->input1 - alu->input2;
				#if defined(CONTROL)
					printf("sub\n");
				#endif
					processor_stat.instruction.sub += 1;
                    break;
                case 0x03:
                    // sra
                    alu->alu_result = alu->input2 >> in->shamt;
                    if (alu->input2 >> 31){ 	// If negative
                        uint32_t temp = 0xFFFFFFFF;
                        temp = temp << (32 - in->shamt);
                        alu->alu_result = alu->alu_result | temp;
                    }
				#if defined(CONTROL)
					printf("sra\n");
				#endif
					processor_stat.instruction.sra += 1;
                    break;
                case 0x26:
                    // xor
                    alu->alu_result = alu->input1 ^ alu->input2;
				#if defined(CONTROL)
					printf("xor\n");
				#endif
					processor_stat.instruction.xor_ctr += 1;
                    break;
                case 0x0C:
                    // syscall
				#if defined(CONTROL)
					printf("syscall\n");
				#endif
                  	syscall(cpu_ctx.GPR[2]);			// $v0 is register 2
                    processor_stat.instruction.syscall += 1;
                    break;
				default:
				#if defined(CONTROL)
					printf("Unimplemented R-type instruction\n");
				#endif
				    processor_stat.instruction.unimp_r_type += 1;
					break;
            }
		break;

        case 0x08:
            // addi
            alu->alu_result = alu->input1 + alu->input2;
		#if defined(CONTROL)
			printf("addi\n");
		#endif
			processor_stat.instruction.addi += 1;
            break;
        case 0x23:
            // lw
            alu->alu_result = alu->input1 + alu->input2;
		#if defined(CONTROL)
			printf("lw\n");
		#endif
			processor_stat.instruction.lw += 1;
            break;
        case 0x2B:
            // sw
            alu->alu_result = alu->input1 + alu->input2;
		#if defined(CONTROL)
			printf("sw\n");
		#endif
			processor_stat.instruction.sw += 1;
            break;
        case 0x0C:
            // andi
            alu->alu_result = alu->input1 & alu->input2;
		#if defined(CONTROL)
			printf("andi\n");
		#endif
			processor_stat.instruction.andi += 1;
            break;
        case 0x04:
            // beq
            alu->alu_result = alu->input1 - alu->input2;
            if (alu->alu_result == 0)
                alu->zero = 1;
            else
                alu->zero = 0;
		#if defined(CONTROL)
			printf("beq\n");
		#endif
			processor_stat.instruction.beq += 1;
			break;
        case 0x05:
            // bne
            alu->alu_result = alu->input1 - alu->input2;
            if (alu->alu_result == 0)
                alu->zero = 0;
            else
                alu->zero = 1;
		#if defined(CONTROL)
			printf("bne\n");
		#endif
			processor_stat.instruction.bne += 1;
            break;
        case 0x02:
            // j
            // do nothing (already calculated in decode stage)
            // mem stage will deal with it
			processor_stat.instruction.j += 1;
		#if defined(CONTROL)
			printf("j\n");
		#endif
            break;
        case 0x03:
            // jal
            // storing PC + 4 in $ra (which is register $31)`
            cpu_ctx.GPR[31] = cpu_ctx.PC + 4;
		#if defined(CONTROL)		
			printf("jal\n");
		#endif
			processor_stat.instruction.jal += 1;
            break;
        case 0x0F:
            // lui
			// input2 = 0x1001
            alu->alu_result = alu->input2 << 16;// TODO: Change lui implementation to handle lw
		#if defined(CONTROL)
			printf("lui\n");
		#endif
			processor_stat.instruction.lui += 1;
            break;
        case 0x0D:
            // ori
            alu->alu_result = alu->input1 | alu->input2;
		#if defined(CONTROL)
			printf("ori\n");
		#endif
			processor_stat.instruction.ori += 1;
            break;
        case 0x0A:
            //slti
            alu->alu_result = (alu->input1 < alu->input2) ? 1 : 0;
		#if defined(CONTROL)
			printf("slti\n");
		#endif
			processor_stat.instruction.slti += 1;
        case 0x0E:
            // xori
            alu->alu_result = alu->input1 ^ alu->input2;
		#if defined(CONTROL)
			printf("xori\n");
		#endif
			processor_stat.instruction.xori += 1;
            break;
		default:
		#if defined(CONTROL)
			printf("Unimplemented I Type Instruction\n");
		#endif
			processor_stat.instruction.unimp_i_type += 1;
            break;
    }
	return 0;
}

int set_and(ID_EX_buffer *in, ALU *alu, AND_GATE *and_gate){

    // set AND gate input
    and_gate->branch = in->branch;
    // the other AND gate input will be set after alu output is calculated

	#if defined (CONTROL) 
		printf("_____________________________________________________________\n");
	#endif

    // set the zero input of the and gate
    and_gate->zero = alu->zero;

    // get the output of the and gate (pc_src)
    and_gate->pc_src = and_gate->branch & and_gate->zero;
	
	return 0;
}

int ex_mem_buffer(ID_EX_buffer* in, ALU *alu, BRANCH_ADD *branch_add, AND_GATE *and_gate, EX_MEM_buffer *out)
{
    out->mem_read = in->mem_read;
    out->mem_write = in->mem_write;
    out->mem_to_reg = in->mem_to_reg;
    out->reg_write = in->reg_write;
    out->jump = in->jump;
    out->write_reg = in->write_reg;
    out->read_data2 = in->read_data2;
    out->jump_address = in->jump_address;
    out->alu_result = alu->alu_result;
    out->branch_address = branch_add->branch_address;
    out->pc_src = and_gate->pc_src;
    return 0;
}
