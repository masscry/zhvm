/**
 * @file interpreter.cpp
 * @author marko
 */

#include <cassert>
#include <zhvm.h>

namespace zhvm {

    void UnpackCommand(uint32_t cmd, uint32_t *opcode, uint32_t *regs, int16_t *imm) {

        union {
            cmd_t c;
            uint32_t i;
        } tcmd;

        tcmd.i = cmd;
        *opcode = tcmd.c.opc;
        regs[CR_DEST] = tcmd.c.dst;
        regs[CR_SRC0] = tcmd.c.sr0;
        regs[CR_SRC1] = tcmd.c.sr1;
        *imm = tcmd.c.imm;

    }

    static int InterpretCommand(memory *mem, uint32_t icmd) {

        uint32_t opc = OP_HLT;
        uint32_t regs[CR_TOTAL] = {0};
        int16_t imm = 0;

        UnpackCommand(icmd, &opc, regs, &imm);

        switch (opc) {
            case OP_HLT:
                return IR_HALT;
            case OP_ADD:
                mem->Set(regs[CR_DEST], mem->Get(regs[CR_SRC0]) + (mem->Get(regs[CR_SRC1]) + imm));
                break;
            case OP_SUB:
                mem->Set(regs[CR_DEST], mem->Get(regs[CR_SRC0]) - (mem->Get(regs[CR_SRC1]) + imm));
                break;
            case OP_MUL:
                mem->Set(regs[CR_DEST], mem->Get(regs[CR_SRC0]) * (mem->Get(regs[CR_SRC1]) + imm));
                break;
            case OP_DIV:
                mem->Set(regs[CR_DEST], mem->Get(regs[CR_SRC0]) / (mem->Get(regs[CR_SRC1]) + imm));
                break;
            case OP_MOD:
                mem->Set(regs[CR_DEST], mem->Get(regs[CR_SRC0]) % (mem->Get(regs[CR_SRC1]) + imm));
                break;
            case OP_CMZ:
                if (mem->Get(regs[CR_SRC0]) == 0) {
                    mem->Set(regs[CR_DEST], mem->Get(regs[CR_SRC1]) + imm);
                }
                break;
            case OP_CMN:
                if (mem->Get(regs[CR_SRC0]) != 0) {
                    mem->Set(regs[CR_DEST], mem->Get(regs[CR_SRC1]) + imm);
                }
                break;
            case OP_LDB:
                mem->Set(regs[CR_DEST],
                        mem->GetByte(mem->Get(regs[CR_SRC0]))
                        + mem->Get(regs[CR_SRC1]) + imm
                        );
                break;
            case OP_LDS:
                mem->Set(regs[CR_DEST],
                        mem->GetShort(mem->Get(regs[CR_SRC0]))
                        + mem->Get(regs[CR_SRC1]) + imm
                        );
                break;
            case OP_LDL:
                mem->Set(regs[CR_DEST],
                        mem->GetLong(mem->Get(regs[CR_SRC0]))
                        + mem->Get(regs[CR_SRC1]) + imm
                        );
                break;
            case OP_LDQ:
                mem->Set(regs[CR_DEST],
                        mem->GetQuad(mem->Get(regs[CR_SRC0]))
                        + mem->Get(regs[CR_SRC1]) + imm
                        );
                break;
            case OP_SVB:
                mem->SetByte(mem->Get(regs[CR_DEST]),
                        mem->Get(regs[CR_SRC0]) + mem->Get(regs[CR_SRC1]) + imm
                        );
                break;
            case OP_SVS:
                mem->SetShort(mem->Get(regs[CR_DEST]),
                        mem->Get(regs[CR_SRC0]) + mem->Get(regs[CR_SRC1]) + imm
                        );
                break;
            case OP_SVL:
                mem->SetLong(mem->Get(regs[CR_DEST]),
                        mem->Get(regs[CR_SRC0]) + mem->Get(regs[CR_SRC1]) + imm
                        );
                break;
            case OP_SVQ:
                mem->SetQuad(mem->Get(regs[CR_DEST]),
                        mem->Get(regs[CR_SRC0]) + mem->Get(regs[CR_SRC1]) + imm
                        );
                break;
            case OP_AND:
                mem->Set(regs[CR_DEST], mem->Get(regs[CR_SRC0]) & (mem->Get(regs[CR_SRC1]) + imm));
                break;
            case OP_OR:
                mem->Set(regs[CR_DEST], mem->Get(regs[CR_SRC0]) | (mem->Get(regs[CR_SRC1]) + imm));
                break;
            case OP_XOR:
                mem->Set(regs[CR_DEST], mem->Get(regs[CR_SRC0]) ^ (mem->Get(regs[CR_SRC1]) + imm));
                break;
            case OP_GR:
                mem->Set(regs[CR_DEST], mem->Get(regs[CR_SRC0]) > (mem->Get(regs[CR_SRC1]) + imm));
                break;
            case OP_LS:
                mem->Set(regs[CR_DEST], mem->Get(regs[CR_SRC0]) < (mem->Get(regs[CR_SRC1]) + imm));
                break;
            case OP_GRE:
                mem->Set(regs[CR_DEST], mem->Get(regs[CR_SRC0]) >= (mem->Get(regs[CR_SRC1]) + imm));
                break;
            case OP_LSE:
                mem->Set(regs[CR_DEST], mem->Get(regs[CR_SRC0]) <= (mem->Get(regs[CR_SRC1]) + imm));
                break;
            case OP_EQ:
                mem->Set(regs[CR_DEST], mem->Get(regs[CR_SRC0]) == (mem->Get(regs[CR_SRC1]) + imm));
                break;
            case OP_NEQ:
                mem->Set(regs[CR_DEST], mem->Get(regs[CR_SRC0]) != (mem->Get(regs[CR_SRC1]) + imm));
                break;
            case OP_CCL:
                return mem->Call(regs[CR_SRC0] + regs[CR_SRC1] + imm);
            case OP_NOP:
                break;
            default:
                return IR_OP_UNKNWN;
        }
        return IR_RUN;
    }

    int Invoke(memory *mem, uint32_t icmd) {
        if (mem == 0) {
            return IR_INVALID_POINTER;
        }
        mem->DropSet();
        return InterpretCommand(mem, icmd);
    }

    int Step(memory* mem) {
        if (mem == 0) {
            return IR_INVALID_POINTER;
        }
        mem->DropSet();
        int result = InterpretCommand(mem, (uint32_t) mem->GetLong(mem->Get(RP)));
        if ((result == IR_RUN)&&(mem->TestSet(RP) == 0)) {
            mem->Set(RP, mem->Get(RP) + sizeof (uint32_t));
        }
        return result;
    }

    int Execute(memory* mem) {

        if (mem == 0) {
            return IR_INVALID_POINTER;
        }

        int result = IR_RUN;
        while (result == IR_RUN) {
            mem->DropSet();
            result = InterpretCommand(mem, (uint32_t) mem->GetLong(mem->Get(RP)));
            if ((result == IR_RUN)&&(mem->TestSet(RP) == 0)) {
                mem->Set(RP, mem->Get(RP) + sizeof (uint32_t));
            }
        }
        return result;
    }

}
