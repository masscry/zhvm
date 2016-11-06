/**
 * @file assembler.h
 * @author marko
 *
 * String manipulations
 *
 */

#pragma once
#ifndef __SMAN_HEADER__
#define __SMAN_HEADER__

#include <cstdint>

namespace zhvm {

    /**
     * Constant char synonym
     */
    typedef const char cchar;

    /**
     * Assemble VM code in form "$X = opcode [$Y, $Z + IMM]".
     *
     * @param cursor string cursor
     * @param result result code in binary form
     * @return 0 on error, cursor after processed code
     */
    cchar* Assemble(cchar *cursor, memory* result, int loglevel);

    /**
     * Return register name by its ID.
     *
     * @param reg register ID.
     * @return null-terminated string of register name
     * @see register
     */
    cchar* GetRegisterName(uint32_t reg);

    /**
     * Return opcode name by its ID.
     *
     * @param opcode opcode ID.
     * @return null-terminated string of opcode name
     * @see opcodes
     */
    cchar* GetOpcodeName(uint32_t opcode);

    /**
     * Get opcode ID.
     *
     * @param text opcode text
     * @return opcode ID
     */
    uint32_t GetOpcode(cchar * text);


    /**
     * Pack opcode, registers and immediate value to VM command.
     *
     * @param opcode VM operation code
     * @param regs array of three registers [DEST, SRC0, SRC1]
     * @param imm immediate value
     */
    uint32_t PackCommand(uint32_t opcode, const uint32_t *regs, int16_t imm);

    /**
     * Unpack opcode, registers and valie from VM command.
     * 
     * @param cmd vm command
     * @param opcode VM operation code
     * @param regs array of three registers
     * @param imm immediate value
     */
    void UnpackCommand(uint32_t cmd, uint32_t *opcode, uint32_t *regs, int32_t * imm);


}

/**
 * Mask to get opcode id from packed code
 */
#define ZHVM_OPMASK(CODE) ((CODE) & ((1 << 6) - 1))

#endif // __SMAN_HEADER__

