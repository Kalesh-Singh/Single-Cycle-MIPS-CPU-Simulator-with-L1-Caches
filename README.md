#  Single Cycle MIPS CPU Simulator with L1 Caches #

This project implements a basic single-cycle processor simulator for a reduced MIPS instruction set and extends this single cycle implementation to simulate a processor with L1 instruction cache (L1-i$) and L1 data cache(L1-d$). 

## Features: ##
  + The single-cycle processor is implemented using the classical 5-stages ”fetch, decode, execute, memory, writeback” with only one instruction going through the stages at a time.
  + The caching is enabled or disabled by the macroprocessor ENABLE_L1_CACHES. This can be done by commenting out #define ENABLE_L1_CACHES in ```cpu.h``` on Line 13. When it is disabled, the simulator proceeds as a regular single-cycle processor. 
  + Configuration of the cache is as follows: the address space is 32-bits,2 KiB for each L1-i$ and L1-d$, Block size of 4 words, L1-i$ is direct-mapped, and L1-d$ is 4-way set associative with True LRU replacement policy using writeback policy for write hits and write-allocate for write misses.
  + The instructions that the simulator supports are: add, addi, and, andi, beq, bne, j, jal, jr, lui, lw, nor, or, ori, slt, slti, sll, srl, sw, sub, sra, xor, xori, syscall 	

## Requirements: ##
  + A C compiler is required to run this program. 
  + 2 text files are required, one containing the text segment of the instructions and one containing the data segment of the instructions in hexadecimal are generated using QtSpim. 
  + The text files are later converted to a binary file using create-prog.
  + The simulator takes the binary file as a command line argument.
  + If you don't have the Linux OS, you will be required to have the Linux sub-system installed on your system to run the program from the command line.

### Download Links: ###
  + QtSpim	http://spimsimulator.sourceforge.net/
  + Linux subsystem https://docs.microsoft.com/en-us/windows/wsl/install-win10

# Instructions: ##

## Making a new test case ##

#### QtSpim: ####
  + Open QtSpim
  + Load the asm code for the instruction being tested
  + Run the program
  + Copy the user text and user data segments to a text editor and save as text.txt and data.txt respectively
  + **Note: You must reinitialize and load the file before running the program again.**

#### Text.txt: ####
  + Example line: [00400000] 8fa40000  lw $4, 0($29)            ; 183: lw $a0 0($sp) # argc 
  + Only keep the hexadecimal 8fa40000
  + Do this for all the lines
  + Replace the first 5 hexadecimal with 00000000
  + Make sure there is a new line after the last hexadecimal

#### Data.txt ####
  + Example line: [10010000] 6c617473 676e696c 726f7720 2164656b  s t a l l i n g   w o r k e d ! 
  + Only keep the hexadecimals -- 6c617473 676e696c 726f7720 2164656b
  + Place each hexadecimal on a new line in the same order
  + Do the same for all the lines while making sure there are no whitespaces following each hexadecimal
  + Make sure there is a new line after the last hexadecimal
  + If there are no such lines, a simple 00000000 and a new line will suffice
  
#### Creating input file for simulator: ####
  + Compile the text and data files to form a binary file
  + Be in the directory of the text and data files of the test case
  + The create-prog file required to compile the text and data files into a binary file is under util directory
  + Hence, the command ```./../util/create-prog data.txt text.txt test_file.sim``` is used

## Running a test case ##

#### Compiling the Simulator: ####
  + If on Linux based system run ```make``` in the main directory.
  + If on Mac based system run ```clang -x c *.c *.h -o single-cycle-with-cache``` in the main directory.

  
#### Running the test files: ####
  + Be in the directory of the simulator binary file, ```single-cycle-with-cache```
  + (Assuming name of test case directory is hw3)
  + Use command -- ```./single-cycle-with-cache test_cases/hw3/hw3.sim``` 

# Test Cases: #
## Details of test case CPU STATS: ##
  + Stalls are counted as useless instructions
  + nop is used to mean 0x00000000 that was either included by the programmer or from the QTSpim conversion and is counted as a useful instruction
  + Cache misses incur 4 stall cycles.

### Name of test case: bne ###
### Purpose of test case: bne ###
To test if the bne instructions work.
Will branch and continue to loop as long as $t0 != $t1, which has the value of 25.
If successful, the program will end with register $t0 holding 25.
### ASM code of test case: bne ###
```asm 
.data
.text
main:
	addi $t0, $0, 5
	addi $t1, $0, 25     
	loop:
		addi $t0, $t0, 5
		addi $t3, $0, 200
		bne $t0, $t1, loop		
	jr $ra
```
### Data.txt of test case: bne ###
```
00000000

```
### Text.txt of test case: bne ###
```
00000000
00000000
00000000
00000000
00000000
0c100009
00000000
3402000a
0000000c
20080005
20090019
21080005
200b00c8
1509fffe
03e00008

```
### CPU STATS of test case: bne ###
With L1 Caches Enabled:
```
CPU STATS:

INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 1
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 0
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 10
        andi = 0
        lui = 0
        ori = 1
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 1

Special Type Instructions:
        lw = 0
        sw = 0
        beq = 0
        bne = 4
        nop = 6

Cache PERFORMANCE:
        Instruction Cache Hits = 20
        Instruction Cache Misses = 4
        Instruction Cache Accesses = 24

        Data Cache Misses = 0
        Data Cache Hits = 0
        Data Cache Accesses = 0

CPI PERFORMANCE:
        Cycles = 40
        Useful Instructions = 24
        Stalls = 16
        CPI = 1.67
```

With L1 Caches Disabled:
```
CPU STATS:

INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 1
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 0
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 10
        andi = 0
        lui = 0
        ori = 1
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 1

Special Type Instructions:
        lw = 0
        sw = 0
        beq = 0
        bne = 4
        nop = 6

CPI PERFORMANCE:
        Cycles = 24
        Useful Instructions = 24
        CPI = 1.00
```


### Name of test case: jump ###
### Purpose of test case: jump ###
To test the jump instruction. If jump is successful, the program will exit succesfully.
### ASM code of test case: jump ###
```asm
.data
.text
main:
	j j2
	
end:
	or $a0, $t2, $0
	li $v0, 1
	syscall
	jr $ra

j1:
	addi $t1, $0, 200
	add $t2, $t1, $t0
	j end
	
j2:
	addi $t0, $0, 500
	j j1
```
### Data.txt of test case: jump ###
```
00000000

```
### Text.txt of test case: jump ###
```
00000000
00000000
00000000
00000000
00000000
0c100009
00000000
3402000a
0000000c
08100011
01402025
34020001
0000000c
03e00008
200900c8
01285020
0810000a
200801f4
0810000e

```
### CPU STATS of test case: jump ###
With L1 Caches Enabled:
```
CPU STATS:

INSTRUCTION COUNTER:

R-Format Instructions:
        add = 1
        and = 0
        jr = 1
        nor = 0
        or = 1
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 1
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 2
        andi = 0
        lui = 0
        ori = 2
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 3
        jal = 1

Special Type Instructions:
        lw = 0
        sw = 0
        beq = 0
        bne = 0
        nop = 6

Cache PERFORMANCE:
        Instruction Cache Hits = 14
        Instruction Cache Misses = 5
        Instruction Cache Accesses = 19

        Data Cache Misses = 0
        Data Cache Hits = 0
        Data Cache Accesses = 0

CPI PERFORMANCE:
        Cycles = 39
        Useful Instructions = 19
        Stalls = 20
        CPI = 2.05
``` 

With L1 Caches Disabled:

```
CPU STATS:

INSTRUCTION COUNTER:

R-Format Instructions:
        add = 1
        and = 0
        jr = 1
        nor = 0
        or = 1
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 1
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 2
        andi = 0
        lui = 0
        ori = 2
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 3
        jal = 1

Special Type Instructions:
        lw = 0
        sw = 0
        beq = 0
        bne = 0
        nop = 6

CPI PERFORMANCE:
        Cycles = 19
        Useful Instructions = 19
        CPI = 1.00

```
### Name of test case: addi ###
### Purpose of test case: addi ###
To test if the addi instructions work. If successful, it will also output 5, which is stored in register $t0.

### ASM code of test case: addi ###
```asm
.data
.text
main:
	addi $t0, $0, 5
    addi $t1, $0, 6
    addi $t2, $0, 11
    addi $t5, $0, 7
    add $a0, $0, $t0
    addi $v0, $0, 1
    addi $t6, $0, 21
    addi $t7, $0, 22
    addi $t8, $0, 23
    addi $s1, $0, 8
    syscall
    addi $t3, $0, 1
    addi $t4, $0, 12
	addi $v0, $0, 10
    addi $s2, $0, 9
    syscall
```
### Data.txt of test case: addi ###
```
00000000

```
### Text.txt of test case: addi ###
```
00000000            
00000000
00000000
00000000
00000000
00000000
00000000
00000000
00000000
20080005
20090006
200a000b
200d0007
00082020
20020001
200e0015
200f0016
20180017
20110008
0000000c
200b0001
200c000c
2002000a
20120009
0000000c
```
### CPU STATS of test case: addi ###
With L1 Caches Enabled:
```
5


CPU STATS:

INSTRUCTION COUNTER:

R-Format Instructions:
        add = 1
        and = 0
        jr = 0
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 1
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 13
        andi = 0
        lui = 0
        ori = 0
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 0

Special Type Instructions:
        lw = 0
        sw = 0
        beq = 0
        bne = 0
        nop = 9

Cache PERFORMANCE:
        Instruction Cache Hits = 18
        Instruction Cache Misses = 7
        Instruction Cache Accesses = 25

        Data Cache Misses = 0
        Data Cache Hits = 0
        Data Cache Accesses = 0

CPI PERFORMANCE:
        Cycles = 53
        Useful Instructions = 25
        Stalls = 28
        CPI = 2.12
```

With L1 Caches Disabled:
```
5


CPU STATS:

INSTRUCTION COUNTER:

R-Format Instructions:
        add = 1
        and = 0
        jr = 0
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 1
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 13
        andi = 0
        lui = 0
        ori = 0
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 0

Special Type Instructions:
        lw = 0
        sw = 0
        beq = 0
        bne = 0
        nop = 9

CPI PERFORMANCE:
        Cycles = 25
        Useful Instructions = 25
        CPI = 1.00
```

### Name of test case: lw_sw ###
### Purpose of test case: lw_sw ###
To test if the lw and sw instructions work. If successful, then it will store correctly the value read by lw from stack memory into another position in the stack memory.
### ASM code of test case: lw_sw ###
```asm
.data
.text
main:
	addi $t0, $0, 5
	addi $sp, $sp, -8
	sw $t0, 0($sp)
	lw $t5, 0($sp)
	sw $t5, 4($sp)
	addi $sp, $sp, 8
	li $v0, 10
	syscall
```
### Data.txt of test case: lw_sw ###
```
00000000

```
### Text.txt of test case: lw_sw ###
```
00000000
00000000
00000000
00000000
00000000
0c100009
00000000
3402000a
0000000c
20080005
23bdfff8
afa80000
8fad0000
afad0004
23bd0008
3402000a
0000000c
```
### CPU STATS of test case: lw_sw ###
With L1 Caches Enabled:
```
CPU STATS:

INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 0
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 0
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 3
        andi = 0
        lui = 0
        ori = 1
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 1

Special Type Instructions:
        lw = 1
        sw = 2
        beq = 0
        bne = 0
        nop = 5

Cache PERFORMANCE:
        Instruction Cache Hits = 9
        Instruction Cache Misses = 5
        Instruction Cache Accesses = 14

        Data Cache Misses = 1
        Data Cache Hits = 2
        Data Cache Accesses = 3

CPI PERFORMANCE:
        Cycles = 38
        Useful Instructions = 14
        Stalls = 24
        CPI = 2.71
```

With L1 Caches Disabled:

```

CPU STATS:

INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 0
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 0
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 3
        andi = 0
        lui = 0
        ori = 1
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 1

Special Type Instructions:
        lw = 1
        sw = 2
        beq = 0
        bne = 0
        nop = 5

CPI PERFORMANCE:
        Cycles = 14
        Useful Instructions = 14
        CPI = 1.00
```

### Name of test case: la ###
### Purpose of test case: la ###
To test if the la instruction works. If successful, it will correctly print the string "awesome" stored in data memory.
### ASM code of test case: la ###
```asm
.data
	name:	.asciiz "awesome"
.text
	main:
		la $a0, name
		addi $v0, $0, 4
		syscall
		jr $ra
```
### Data.txt of test case: la ###
```
73657761
00656d6f

```
### Text.txt of test case: la ###
```
00000000
00000000
00000000
00000000
00000000
0c100009
00000000
3402000a
0000000c
3c041001
20020004
0000000c
03e00008


```
### CPU STATS of test case: la ###
With L1 Caches Enabled:
```
awesome


CPU STATS:

INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 1
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 1
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 1
        andi = 0
        lui = 1
        ori = 1
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 1

Special Type Instructions:
        lw = 0
        sw = 0
        beq = 0
        bne = 0
        nop = 6

Cache PERFORMANCE:
        Instruction Cache Hits = 9
        Instruction Cache Misses = 4
        Instruction Cache Accesses = 13

        Data Cache Misses = 0
        Data Cache Hits = 0
        Data Cache Accesses = 0

CPI PERFORMANCE:
        Cycles = 29
        Useful Instructions = 13
        Stalls = 16
        CPI = 2.23
```

With L1 Caches Disabled:

```
awesome


CPU STATS:

INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 1
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 1
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 1
        andi = 0
        lui = 1
        ori = 1
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 1

Special Type Instructions:
        lw = 0
        sw = 0
        beq = 0
        bne = 0
        nop = 6

CPI PERFORMANCE:
        Cycles = 13
        Useful Instructions = 13
        CPI = 1.00
```
### Name of test case: hw3 ###
### Purpose of test case: hw3 ###
This is a general test case that has most instructions of the simulators. If successful, it will terminate successfully since it uses jr to exit from main.
### ASM code of test case: hw3 ###
```asm
.data
.text
fun:     # int fun(int s) { 
	addi $sp, $sp, -8 
	sw $ra, 0($sp) 
	sw $a0, 4($sp) 
	slt $t0, $0, $a0 # if (s > 0) 
	beq $t0, $0, base # { 
	addi $a0, $a0, -1 # s--; 
	jal fun   #  r = fun(s); 
	j done   # } 
base:
	addi $v0, $zero, 0 # else r = 0;
done:
	lw $ra, 0($sp)   
	lw $a0, 4($sp) 
	addi $sp, $sp, 8 
	add $v0, $v0, $a0 # r = r + s;
	jr $ra   # return r; }
main:    # int main() { 
	addi $sp, $sp, -8  
	sw $ra, 0($sp) 
	sw $s0, 4($sp) 
	addi $s0, $zero, 0 # x = 0 
	addi $a0, $zero, 2 # i = 2
loop:
	slt $t0, $0, $a0 # while ( i > 0) 
	beq $t0, $0, end # { 
	jal fun 
	add $s0, $s0, $v0 #   x = x + fun(i); 
	addi $a0, $a0, -1 #   i--; 
	j loop   # }
end:
	addi $v0, $s0, 0 
	lw $s0, 4($sp) 
	lw $ra, 0($sp) 
	addi $sp, $sp, 8 
	jr $ra # return x; }

```
### Data.txt of test case: hw3 ###
```
00000000

```
### Text.txt of test case: hw3 ###
```
00000000
00000000
00000000
00000000
00000000
0c100017
00000000
3402000a
0000000c
23bdfff8
afbf0000
afa40004
0004402a
11000004
2084ffff
0c100009
08100012
20020000
8fbf0000
8fa40004
23bd0008
00441020
03e00008
23bdfff8
afbf0000
afb00004
20100000
20040002
0004402a
11000005
0c100009
02028020
2084ffff
0810001c
22020000
8fb00004
8fbf0000
23bd0008
03e00008

```
### CPU STATS of test case: hw3 ###
With L1 Caches Enabled:
```
CPU STATS:

INSTRUCTION COUNTER:

R-Format Instructions:
        add = 7
        and = 0
        jr = 6
        nor = 0
        or = 0
        slt = 8
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 0
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 22
        andi = 0
        lui = 0
        ori = 1
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 5
        jal = 6

Special Type Instructions:
        lw = 12
        sw = 12
        beq = 8
        bne = 0
        nop = 6

Cache PERFORMANCE:
        Instruction Cache Hits = 84
        Instruction Cache Misses = 10
        Instruction Cache Accesses = 94

        Data Cache Misses = 2
        Data Cache Hits = 22
        Data Cache Accesses = 24

CPI PERFORMANCE:
        Cycles = 142
        Useful Instructions = 94
        Stalls = 48
        CPI = 1.51
```

With L1 Caches Disabled:
```

CPU STATS:

INSTRUCTION COUNTER:

R-Format Instructions:
        add = 7
        and = 0
        jr = 6
        nor = 0
        or = 0
        slt = 8
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 0
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 22
        andi = 0
        lui = 0
        ori = 1
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 5
        jal = 6

Special Type Instructions:
        lw = 12
        sw = 12
        beq = 8
        bne = 0
        nop = 6

CPI PERFORMANCE:
        Cycles = 94
        Useful Instructions = 94
        CPI = 1.00
```

### Name of test case: jal ###
### Purpose of test case: jal ###
To test the jal and jr instructions for the single-cycle processor. If successful, it outputs the strings "we jumped to loop2" and "jal worked for god's sake", as well as the integers 11 and 11.
### ASM code of test case: jal ###
```asm
.data
	worked: .asciiz "jal worked for god's sake\n"
	jumped: .asciiz "we jumped to loop2\n"
.text
	main:
		addi $sp, $sp, -8
		sw $ra, 0($sp)
		jal loop1
		lw $ra, 0($sp)
		addi $v0, $0, 1
		addi $a0, $0, 11
		syscall
		jr $ra
		loop1:
			sw $ra, 4($sp)
			jal loop2
			addi $v0, $0, 4 
			la $a0, worked
			syscall
			lw $ra, 4($sp)
			addi $v0, $0, 1
			addi $a0, $0, 11
			syscall
			jr $ra
	loop2:
		addi $v0, $0, 4
		la $a0, jumped
		syscall
		jr $ra
```
### Data.txt of test case: jal ###
```
206c616a
6b726f77
66206465
6720726f
7327646f
6b617320
77000a65
756a2065
6465706d
206f7420
706f6f6c
00000a32

```
### Text.txt of test case: jal ###
```
00000000
00000000
00000000
00000000
00000000
0c100009
00000000
3402000a
0000000c
23bdfff8
afbf0000
0c100011
8fbf0000
20020001
2004000b
0000000c
03e00008
afbf0004
0c10001b
20020004
3c041001
0000000c
8fbf0004
20020001
2004000b
0000000c
03e00008
20020004
3c011001
3424001b
0000000c
03e00008

```
### CPU STATS of test case: jal ###
With L1 Caches Enabled:

```
we jumped to loop2
jal worked for god's sake
1111


CPU STATS:

INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 3
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 4
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 7
        andi = 0
        lui = 2
        ori = 2
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 3

Special Type Instructions:
        lw = 2
        sw = 2
        beq = 0
        bne = 0
        nop = 6

Cache PERFORMANCE:
        Instruction Cache Hits = 24
        Instruction Cache Misses = 8
        Instruction Cache Accesses = 32

        Data Cache Misses = 1
        Data Cache Hits = 3
        Data Cache Accesses = 4

CPI PERFORMANCE:
        Cycles = 68
        Useful Instructions = 32
        Stalls = 36
        CPI = 2.12

```

With L1 Caches Disabled:

```
we jumped to loop2
jal worked for god's sake
1111


CPU STATS:

INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 3
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 4
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 7
        andi = 0
        lui = 2
        ori = 2
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 3

Special Type Instructions:
        lw = 2
        sw = 2
        beq = 0
        bne = 0
        nop = 6

CPI PERFORMANCE:
        Cycles = 32
        Useful Instructions = 32
        CPI = 1.00
```
# Errors: #
  + As of now, the program is running without any errors.

# Problems: #
  + As of now, there aren't any problems in the program. 
  
# Workload Distribution: #
  + Work was distributed equally among all team members.
  + Kaleshwar Singh partially implemented the instruction memory cache, created Makefile and debugged data memory cache.
  + Hencching Limbu partially implemented instruction memory cache and CPU statistics.
  + Zaykha Kyaw San implemented the data memory cache.  
  + Mahia Tasneem partially implemented data memory cache.
  + Whitney Griffith debugged, created and tested test cases and README.md.

# Contributors: #
  + Kaleshwar Singh 
  + Mahia Tasneem
  + Henchhing Limbu
  + Zaykha Kyaw San
  + Whitney Griffith