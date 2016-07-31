/**
 * @file zconst.h
 * @author marko
 */

#pragma once
#ifndef __ZCONST_HEADER__
#define __ZCONST_HEADER__

namespace zhvm {

    /**
     * Operation codes list
     */
    enum opcodes {
        OP_HALT = 0,    ///< 0x00 HALT VM
        OP_ADD,         ///< 0x01 D = S0 + (S1 + IM)
        OP_SUB,         ///< 0x02 D = S0 - (S1 + IM)
        OP_MUL,         ///< 0x03 D = S0 * (S1 + IM)
        OP_DIV,         ///< 0x04 D = S0 / (S1 + IM)
        OP_MOD,         ///< 0x05 D = S0 % (S1 + IM)
        OP_CMZ,         ///< 0x06 Conditional move zero
        OP_CMN,         ///< 0x07 Conditional move non-zero

        OP_LDB,         ///< 0x08 D = (1 byte)mem[S0+S1+IM]
        OP_LDS,         ///< 0x09 D = (2 bytes)mem[S0+S1+IM]
        OP_LDL,         ///< 0x0A D = (4 bytes)mem[S0+S1+IM]
        OP_LDQ,         ///< 0x0B D = (8 bytes)mem[S0+S1+IM]

        OP_SVB,         ///< 0x0C mem[D+S1+IM] = (1 byte)S0
        OP_SVS,         ///< 0x0D mem[D+S1+IM] = (2 bytes)S0
        OP_SVL,         ///< 0x0E mem[D+S1+IM] = (4 bytes)S0
        OP_SVQ,         ///< 0x0F mem[D+S1+IM] = (8 bytes)S0

        OP_AND,         ///< 0x10 D = S0 & (S1 + IM)
        OP_OR,          ///< 0x11 D = S0 | (S1 + IM)
        OP_XOR,         ///< 0x12 D = S0 ^ (S1 + IM)

        OP_TOTAL,
        OP_UNKNOWN = 0xFFFFFFFF, ///< UNKNOWN CODE
    };

    /**
     * Register ID codes
     */
    enum registers {
        RZ = 0x0,       ///< ZERO REGISTER
        RA = 0x1,       ///<    A REGISTER
        RB = 0x2,       ///<    B REGISTER
        RC = 0x3,       ///<    C REGISTER
        R0 = 0x4,       ///<    0 EXTENDED REGISTER
        R1 = 0x5,       ///<    1 EXTENDED REGISTER
        R2 = 0x6,       ///<    2 EXTENDED REGISTER
        R3 = 0x7,       ///<    3 EXTENDED REGISTER
        R4 = 0x8,       ///<    4 EXTENDED REGISTER
        R5 = 0x9,       ///<    5 EXTENDED REGISTER
        R6 = 0xA,       ///<    6 EXTENDED REGISTER
        R7 = 0xB,       ///<    7 EXTENDED REGISTER
        R8 = 0xC,       ///<    8 EXTENDED REGISTER
        RS = 0xD,       ///< STACK POINTER REGISTER
        RD = 0xE,       ///<  DATA POINTER REGISTER
        RP = 0xF,       ///<  CODE POINTER REGISTER
        RTOTAL = 0x10,   ///< TOTAL REGISTER COUNT (16)
        RNOTREG = 0x11,   ///< NOT A REGISTER ERROR CODE
        RUNKNWN = 0x12,   ///< UNKNOWN REGISTER ERROR CODE
    };

    /**
     * Invoke and Execute functions return codes.
     * @see Invoke
     * @see Execute
     */
    enum invoke_result {
        IR_RUN = 0,        ///< VM still running
        IR_HALT,           ///< VM in halt state
        IR_OP_UNKNWN,      ///< VM hit unknown operand
        IR_INVALID_POINTER ///< Invalid memory object pointer
    };

    /**
     * Command adressing three registers.
     */
    enum command_registers {
        CR_DEST, ///< 4-BIT DESTINATION REGISTER
        CR_SRC0, ///< 4-BIT FIRST SOURCE REGISTER
        CR_SRC1, ///< 2-BIT SECOND SOURCE REGISTER (ONLY RZ, RA, RB, RC)
        CR_TOTAL ///< Total command register count
    };

#define ZHVM_OPCODE_SIZE (6)
#define ZHVM_RGDEST_SIZE (4)
#define ZHVM_RGSRC0_SIZE (4)
#define ZHVM_RGSRC1_SIZE (2)
#define ZHVM_IMMVAL_SIZE (16)

#define ZHVM_OPCODE_OFFSET (0)
#define ZHVM_RGDEST_OFFSET (ZHVM_OPCODE_SIZE)
#define ZHVM_RGSRC0_OFFSET (ZHVM_RGDEST_OFFSET+ZHVM_RGDEST_SIZE)
#define ZHVM_RGSRC1_OFFSET (ZHVM_RGSRC0_OFFSET+ZHVM_RGSRC0_SIZE)
#define ZHVM_IMMVAL_OFFSET (ZHVM_RGSRC1_OFFSET+ZHVM_RGSRC1_SIZE)

#define ZHVM_OPCODE_MASK ((1<<ZHVM_OPCODE_SIZE)-1)
#define ZHVM_RGDEST_MASK ((1<<ZHVM_RGDEST_SIZE)-1)
#define ZHVM_RGSRC0_MASK ((1<<ZHVM_RGSRC0_SIZE)-1)
#define ZHVM_RGSRC1_MASK ((1<<ZHVM_RGSRC1_SIZE)-1)
#define ZHVM_IMMVAL_MASK ((1<<ZHVM_IMMVAL_SIZE)-1)

}

#endif // __ZCONST_HEADER__

