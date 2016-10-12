/**
 * @file constants.h
 * @author marko
 *
 * Common structures and constants 
 *
 */

#pragma once
#ifndef __ZCONST_HEADER__
#define __ZCONST_HEADER__

#include <cstdint>

/**
 * ZHVM image identifier.
 */
#define ZHVM_MEMORY_FILE_MAGIC (0xD0FA5534)

/**
 * ZHVM versions.
 * 
 * 1) Initial version with 16-bit immediate value
 * 2) Version with 14-bit immediate value
 * 3) Harvard architecture adopted
 * 4) Save registers state in VM image
 * 5) Add "not" opcode
 * 
 */
#define ZHVM_VM_VERSION (5)


namespace zhvm {

#pragma pack(push, 1)

    /**
     * ZHVM command.
     */
    struct cmd_t {
        uint32_t opc : 6; ///<  6-bit Operation 
        uint32_t dst : 4; ///<  4-bit Destination register
        uint32_t sr0 : 4; ///<  4-bit Source 0 register
        uint32_t sr1 : 4; ///<  4-bit Source 1 register
        int32_t imm : 14; ///< 14-bit Immediate value
    };

#pragma pack(pop)

    /**
     * Operation codes list.
     */
    enum opcodes {
        OP_HLT = 0x00, ///< 0x00 HALT VM
        OP_ADD = 0x01, ///< 0x01 D = S0 + (S1 + IM)
        OP_SUB = 0x02, ///< 0x02 D = S0 - (S1 + IM)
        OP_MUL = 0x03, ///< 0x03 D = S0 * (S1 + IM)
        OP_DIV = 0x04, ///< 0x04 D = S0 / (S1 + IM)
        OP_MOD = 0x05, ///< 0x05 D = S0 % (S1 + IM)
        OP_CMZ = 0x06, ///< 0x06 Conditional move zero
        OP_CMN = 0x07, ///< 0x07 Conditional move non-zero

        OP_LDB = 0x08, ///< 0x08 D = (1 byte)mem[S0]+S1+IM
        OP_LDS = 0x09, ///< 0x09 D = (2 bytes)mem[S0]+S1+IM
        OP_LDL = 0x0A, ///< 0x0A D = (4 bytes)mem[S0]+S1+IM
        OP_LDQ = 0x0B, ///< 0x0B D = (8 bytes)mem[S0]+S1+IM

        OP_SVB = 0x0C, ///< 0x0C mem[D] = (1 byte)S0+S1+IM
        OP_SVS = 0x0D, ///< 0x0D mem[D] = (2 bytes)S0+S1+IM
        OP_SVL = 0x0E, ///< 0x0E mem[D] = (4 bytes)S0+S1+IM
        OP_SVQ = 0x0F, ///< 0x0F mem[D] = (8 bytes)S0+S1+IM

        OP_AND = 0x10, ///< 0x10 D = S0 & (S1 + IM)
        OP_OR = 0x11, ///< 0x11 D = S0 | (S1 + IM)
        OP_XOR = 0x12, ///< 0x12 D = S0 ^ (S1 + IM)

        OP_GR = 0x13, ///< 0x13 D = S0 > (S1 + IM)
        OP_LS = 0x14, ///< 0x14 D = S0 < (S1 + IM)
        OP_GRE = 0x15, ///< 0x15 D = S0 >= (S1 + IM)
        OP_LSE = 0x16, ///< 0x16 D = S0 <= (S1 + IM)
        OP_EQ = 0x17, ///< 0x17 D = S0 == (S1 + IM)
        OP_NEQ = 0x18, ///< 0x18 D = S0 != (S1 + IM)

        OP_CCL = 0x19, ///< 0x19 Call C function
        OP_CPY = 0x1A, ///< 0x1A memmove(D, S0, S1 + IM)
        OP_CMP = 0x1B, ///< 0x1B D = memcmp(D, S0, S1 + IM)
        OP_ZCL = 0x1C, ///< 0x1C Call ZHVM function
        OP_RET = 0x1D, ///< 0x1D Return from ZHVM function
        OP_NOT = 0x1E, ///< 0x1E Logical not
        OP_R1F = 0x1F, ///< 0x1F RESERVED
        OP_R20 = 0x20, ///< 0x20 RESERVED
        OP_R21 = 0x21, ///< 0x21 RESERVED
        OP_R22 = 0x22, ///< 0x22 RESERVED
        OP_R23 = 0x23, ///< 0x23 RESERVED
        OP_R24 = 0x24, ///< 0x24 RESERVED
        OP_R25 = 0x25, ///< 0x25 RESERVED
        OP_R26 = 0x26, ///< 0x26 RESERVED
        OP_R27 = 0x27, ///< 0x27 RESERVED
        OP_R28 = 0x28, ///< 0x28 RESERVED
        OP_R29 = 0x29, ///< 0x29 RESERVED
        OP_R2A = 0x2A, ///< 0x2A RESERVED
        OP_R2B = 0x2B, ///< 0x2B RESERVED
        OP_R2C = 0x2C, ///< 0x2C RESERVED
        OP_R2D = 0x2D, ///< 0x2D RESERVED
        OP_R2E = 0x2E, ///< 0x2E RESERVED
        OP_R2F = 0x2F, ///< 0x2F RESERVED
        OP_R30 = 0x30, ///< 0x30 RESERVED
        OP_R31 = 0x31, ///< 0x31 RESERVED
        OP_R32 = 0x32, ///< 0x32 RESERVED
        OP_R33 = 0x33, ///< 0x33 RESERVED
        OP_R34 = 0x34, ///< 0x34 RESERVED
        OP_R35 = 0x35, ///< 0x35 RESERVED
        OP_R36 = 0x36, ///< 0x36 RESERVED
        OP_R37 = 0x37, ///< 0x37 RESERVED
        OP_R38 = 0x38, ///< 0x38 RESERVED
        OP_R39 = 0x39, ///< 0x39 RESERVED
        OP_R3A = 0x3A, ///< 0x3A RESERVED
        OP_R3B = 0x3B, ///< 0x3B RESERVED
        OP_R3C = 0x3C, ///< 0x3C RESERVED
        OP_R3D = 0x3D, ///< 0x3D RESERVED
        OP_R3E = 0x3E, ///< 0x3E RESERVED

        OP_NOP = 0x3F, ///< 0x3F DO NOTHING (WASTE CYCLE)

        OP_TOTAL = 0x40,
        OP_UNKNOWN = 0xFFFFFFFF, ///< UNKNOWN CODE
    };

    /**
     * Register ID codes.
     */
    enum registers {
        RZ = 0x0, ///< ZERO REGISTER
        RA = 0x1, ///<    A REGISTER
        RB = 0x2, ///<    B REGISTER
        RC = 0x3, ///<    C REGISTER
        R0 = 0x4, ///<    0 REGISTER
        R1 = 0x5, ///<    1 REGISTER
        R2 = 0x6, ///<    2 REGISTER
        R3 = 0x7, ///<    3 REGISTER
        R4 = 0x8, ///<    4 REGISTER
        R5 = 0x9, ///<    5 REGISTER
        R6 = 0xA, ///<    6 REGISTER
        R7 = 0xB, ///<    7 REGISTER
        R8 = 0xC, ///<    8 REGISTER
        RS = 0xD, ///< STACK POINTER REGISTER
        RD = 0xE, ///<  DATA POINTER REGISTER
        RP = 0xF, ///<  CODE POINTER REGISTER
        RTOTAL = 0x10, ///< TOTAL REGISTER COUNT (16)
        RNOTREG = 0x11, ///< NOT A REGISTER ERROR CODE
        RUNKNWN = 0x12, ///< UNKNOWN REGISTER ERROR CODE
    };

    /**
     * Invoke and Execute functions return codes.
     * @see Invoke
     * @see Execute
     */
    enum invoke_result {
        IR_RUN = 0, ///< VM still running
        IR_HALT, ///< VM in halt state
        IR_OP_UNKNWN, ///< VM hit unknown operand
        IR_INVALID_POINTER ///< Invalid memory object pointer
    };

    /**
     * Command adressing three registers.
     */
    enum command_registers {
        CR_DEST, ///< 4-BIT DESTINATION REGISTER
        CR_SRC0, ///< 4-BIT FIRST SOURCE REGISTER
        CR_SRC1, ///< 4-BIT SECOND SOURCE REGISTER
        CR_TOTAL ///< Total command register count
    };

    /**
     * Max immediate value in command.
     */
    const int16_t ZHVM_IMMVAL_MAX = 1 << 13;

    /**
     * Min immediate value in command.
     */
    const int16_t ZHVM_IMMVAL_MIN = -(1 << 13);

    /**
     * Maximum vm functions.
     */
    const uint32_t ZHVM_CFUNC_ARRAY_SIZE = 0x10;

    /**
     * Predefined standard vm functions.
     */
    enum cfunc_name {
        CN_PUT = 0, ///< Put value to console
        CN_GET = 1, ///< Read value from console
        CN_PUTC = 2, ///< Put char to stdout
        CN_GETC = 3, ///< Get char from stdin
        CN_TOTAL = ZHVM_CFUNC_ARRAY_SIZE
    };

    /**
     * VM main registers type
     */
    typedef int64_t reg_t;

}

#endif // __ZCONST_HEADER__

