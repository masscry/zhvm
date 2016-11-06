/**
 * @file sman.cpp
 * @author marko
 */

#include <cstdlib>
#include <cctype>
#include <cstring>
#include <climits>

#include <zhvm.h>
#include <zhvm/cmplv2.class.h>

namespace {

    using namespace zhvm;

    /**
     * Register names
     */
    cchar* regnames[RTOTAL + 1] = {
        "$Z",
        "$A",
        "$B",
        "$C",
        "$0",
        "$1",
        "$2",
        "$3",
        "$4",
        "$5",
        "$6",
        "$7",
        "$8",
        "$S",
        "$D",
        "$P",
        0
    };

    /**
     * Operation code names
     */
    cchar *optexts[OP_TOTAL] = {
        "hlt",
        "add",
        "sub",
        "mul",
        "div",
        "mod",
        "cmz",
        "cmn",
        "ldb",
        "lds",
        "ldl",
        "ldq",
        "svb",
        "svs",
        "svl",
        "svq",
        "and",
        "or",
        "xor",
        "gr",
        "ls",
        "gre",
        "lse",
        "eq",
        "neq",
        "cll",
        "cpy",
        "cmp",
        "zcl",
        "ret",
        "not",
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        "nop"
    };

}


namespace zhvm {

    uint32_t GetOpcode(cchar* text) {
        for (uint32_t index = OP_HLT; index < OP_TOTAL; ++index) {
            if ((optexts[index] != 0)&&(strcmp(optexts[index], text) == 0)) {
                return index;
            }
        }
        return OP_UNKNOWN;
    }

    uint32_t PackCommand(uint32_t opcode, const uint32_t *regs, int16_t imm) {

        uint32_t result = 0;

        result |= opcode & ((1 << 6) - 1);
        result |= (regs[CR_DEST] & ((1 << 4) - 1)) << 6;
        result |= (regs[CR_SRC0] & ((1 << 4) - 1)) << 10;
        result |= (regs[CR_SRC1] & ((1 << 4) - 1)) << 14;
        result |= (imm & ((1 << 14) - 1)) << 18;

        return result;
    }

    cchar* Assemble(cchar *cursor, memory* result, int loglevel) {

        if (cursor == 0) {
            return 0;
        }

        if (result == 0) {
            return 0;
        }

        cmplv2 compiler(cursor, result);
        compiler.SetLogLevel(loglevel);

        if (compiler() == TT2_EOF) {
            return cursor + strlen(cursor);
        }
        return 0;
    }

    cchar* GetRegisterName(uint32_t reg) {
        if (reg < RTOTAL) {
            return regnames[reg];
        }
        return "$?";
    }

    cchar* GetOpcodeName(uint32_t opcode) {
        if (opcode < OP_TOTAL) {
            return optexts[opcode];
        }
        return "???";
    }

}
