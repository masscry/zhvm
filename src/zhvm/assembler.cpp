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
        
        cmd_t cmd;

        cmd.opc = opcode;
        cmd.dst = regs[CR_DEST];
        cmd.sr0 = regs[CR_SRC0];
        cmd.sr1 = regs[CR_SRC1];
        cmd.imm = imm;
        return *reinterpret_cast<uint32_t*>(&cmd);
    }

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
