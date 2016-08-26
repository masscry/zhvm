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

        union {
            cmd_t c;
            uint32_t i;
        } cmd;
        cmd.i = 0;

        cmd.c.opc = opcode;
        cmd.c.dst = regs[CR_DEST];
        cmd.c.sr0 = regs[CR_SRC0];
        cmd.c.sr1 = regs[CR_SRC1];
        cmd.c.imm = imm;
        return cmd.i;
    }

    // $dest opcode $s0, $s1, imm

    cchar* Assemble(cchar *cursor, memory* result) {

        if (cursor == 0) {
            return 0;
        }

        if (result == 0) {
            return 0;
        }

        cmplv2 compiler(cursor, result);

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

}
