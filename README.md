ZH Virtual Machine
==================

Overview
--------

ZHVM - is a registers based RISC virtual machine (Harvard memory organization) with 32-bit code length
and 64-bit registers. 


Requirements
------------

ZHVM based on standard C library and STL.

Assembler requires FLEX 2.6 to be installed in system.

Project structure
-----------------

ZHVM consists of two parts:

* libzhvm.a - library implements VM and assembler
* libzyaml.a - library implements JSON/YAML parsing
* repl - Read-Eval-Print-Loop application to work with ZHVM 
* cmplv2 - Standalone ZHVM assembler

Compilation
-----------

ZHVM project requires CMake 3.0 to be built.

Examples
--------

There are some ZHVM code examples in ${SOURCE_DIRECTORY}/share.

* test-001.zsf - simple consters program
* test-002.zsf - fibbonachi numbers calculator
* test-003.zsf - simple function invocation 

Command structure
-----------------

VM process 32-bit commands. Each command encodes operation code, two source 
registers, one destination register and 14-bit immediate constant.

Registers
---------

VM has 15+1 general usage registers:

* RZ - Zero register. Always return zero, even if non-zero value was written
* RA - A(ccumulator) register.
* RB - B(ase) register.
* RC - C(ounter) register.
* R0-R8 - Argument registers.
* RS - S(tack) top register.
* RD - D(ata) register.
* RP - P(rogram) pointer. Incremented by 4 bytes each cycle.

Any register can be used in any operation, but RP and RZ have special meaning.

Operation codes
---------------

VM uses 6-bit operation code length, which can be used to code 64 commands.

Current VM version has several reserved for future use codes. If any of reserved
codes are found, VM halts with error.

Valid operation codes:

* hlt - halt vm.
* add - add two numbers. [$d = $s0 + ($s1 + imm)]
* sub - substract two numbers. [$d = $s0 - ($s1 + imm)]
* mul - two numbers multiplications. [$d = $s0 * ($s1 + imm)]
* div - two numbers division. [$d = $s0 / ($s1 + imm)]
* mod - two numbers division remainder. [$d = $s0 % ($s1 + imm)]
* cmz - conditional move zero. [if ($s0 == 0) $d = ($s1 + imm)]
* cmn - conditional move non-zero. [if ($s0 != 0) $d = ($s1 + imm)]
* ldb - load 1-byte.  [$d = mem[S0] + S1 + imm] 
* lds - load 2-bytes. [$d = mem[S0] + S1 + imm] 
* ldl - load 4-bytes. [$d = mem[S0] + S1 + imm]
* ldq - load 8-bytes. [$d = mem[S0] + S1 + imm]
* svb - store 1-byte. [mem[$d] = S0 + S1 + imm]
* svs - store 2-bytes. [mem[$d] = S0 + S1 + imm]
* svl - store 4-bytes. [mem[$d] = S0 + S1 + imm]
* svq - store 8-bytes. [mem[$d] = S0 + S1 + imm]
* and - bitwise and. [$d = $s0 & ($s1 + imm)]
* or  - bitwise or. [$d = $s0 | ($s1 + imm)]
* xor - bitwise xor. [$d = $s0 ^ ($s1 + imm)]
* gr - is greater. [$d = $s0 > ($s1 + imm)]
* ls - is less. [$d = $s0 < ($s1 + imm)]
* gre - is greater or equal. [$d = $s0 >= ($s1 + imm)]
* lse - is less or equal. [$d = $s0 <= ($s1 + imm)]
* eq - is equal. [$d = $s0 == ($s1 + imm)]
* neq - is not equal. [$d = $s0 != ($s1 + imm)]
* ccl - call c-func. [cfuncs[$s0 + $s1 + imm]\(\)]
* cpy - copy bytes. [memcpy($d, $s0, S1 + imm)]
* cmp - compare bytes. [$d = memcpy($d, $s0, S1 + imm)]
* nop - do nothing.

C functions
-----------

C functions can be called from VM. Current implementation expects that only 16 
function available.

Standard functions: 

1. print RA to console. [`ccl[,0]`]
2. scan from console to RA. [`ccl[,1]`]

VM image structure
------------------

ZHVM image starts with 16 bytes header:

* [4 bytes] Magic number. [0xD0FA5534] 
* [4 bytes] VM version. [current and only version is 3]
* [4 bytes] VM code byte length. [actual VM code length]
* [4 bytes] VM data byte length. [actual VM data length]

Immediatly after follows the very body of VM image.

After image body sdbm hash of header and body are saved.

