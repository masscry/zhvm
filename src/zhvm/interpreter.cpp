/**
 * @file interpreter.cpp
 * @author marko
 */

#include <zhvm.h>

#define ZHVM_GET_OPCODE(cmd, opcode) (opcode = (cmd&(ZHVM_OPCODE_MASK<<ZHVM_OPCODE_OFFSET))>>ZHVM_OPCODE_OFFSET)
#define ZHVM_GET_RGDEST(cmd, rgdest) (rgdest = (cmd&(ZHVM_RGDEST_MASK<<ZHVM_RGDEST_OFFSET))>>ZHVM_RGDEST_OFFSET)
#define ZHVM_GET_RGSRC0(cmd, rgsrc0) (rgsrc0 = (cmd&(ZHVM_RGSRC0_MASK<<ZHVM_RGSRC0_OFFSET))>>ZHVM_RGSRC0_OFFSET)
#define ZHVM_GET_RGSRC1(cmd, rgsrc1) (rgsrc1 = (cmd&(ZHVM_RGSRC1_MASK<<ZHVM_RGSRC1_OFFSET))>>ZHVM_RGSRC1_OFFSET)
#define ZHVM_GET_IMMVAL(cmd, immval) (immval = (cmd&(ZHVM_IMMVAL_MASK<<ZHVM_IMMVAL_OFFSET))>>ZHVM_IMMVAL_OFFSET)

namespace zhvm {

    inline void UnpackCommand(uint32_t cmd, uint32_t *opcode, uint32_t *regs, int16_t *imm) {
        ZHVM_GET_OPCODE(cmd, *opcode);
        ZHVM_GET_RGDEST(cmd, regs[CR_DEST]);
        ZHVM_GET_RGSRC0(cmd, regs[CR_SRC0]);
        ZHVM_GET_RGSRC1(cmd, regs[CR_SRC1]);
        ZHVM_GET_IMMVAL(cmd, *imm);
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
                mem->Set(
                        regs[CR_DEST],
                        mem->GetByte(
                                mem->Get(regs[CR_SRC0])
                                + mem->Get(regs[CR_SRC1])
                                + imm
                        )
                );
                break;
            case OP_LDS:
                mem->Set(
                        regs[CR_DEST],
                        mem->GetShort(
                                mem->Get(regs[CR_SRC0])
                                + mem->Get(regs[CR_SRC1])
                                + imm
                        )
                );
                break;
            case OP_LDL:
                mem->Set(
                        regs[CR_DEST],
                        mem->GetLong(
                                mem->Get(regs[CR_SRC0])
                                + mem->Get(regs[CR_SRC1])
                                + imm
                        )
                );
                break;
            case OP_LDQ:
                mem->Set(
                        regs[CR_DEST],
                        mem->GetQuad(
                                mem->Get(regs[CR_SRC0])
                                + mem->Get(regs[CR_SRC1])
                                + imm
                        )
                );
                break;
            case OP_SVB:
                mem->SetByte(
                        mem->Get(regs[CR_DEST])
                        + mem->Get(regs[CR_SRC1])
                        + imm,
                        mem->Get(regs[CR_SRC0])
                );
                break;
            case OP_SVS:
                mem->SetShort(
                        mem->Get(regs[CR_DEST])
                        + mem->Get(regs[CR_SRC1])
                        + imm,
                        mem->Get(regs[CR_SRC0])
                );
                break;
            case OP_SVL:
                mem->SetLong(
                        mem->Get(regs[CR_DEST])
                        + mem->Get(regs[CR_SRC1])
                        + imm,
                        mem->Get(regs[CR_SRC0])
                );
                break;
            case OP_SVQ:
                mem->SetQuad(
                        mem->Get(regs[CR_DEST])
                        + mem->Get(regs[CR_SRC1])
                        + imm,
                        mem->Get(regs[CR_SRC0])
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

        int result = InterpretCommand(mem, icmd);
        if (result == IR_RUN){
            mem->SetLong(mem->Get(RP), (int32_t) icmd);
            mem->Set(RP, mem->Get(RP) + sizeof(uint32_t));
        }
        return result;
    }
    
    int Execute(memory* mem){

        if (mem == 0) {
            return IR_INVALID_POINTER;
        }

        int result = IR_RUN;
        while (result == IR_RUN){
            result = InterpretCommand(mem, (uint32_t)mem->GetLong(mem->Get(RP)));
            if (result == IR_RUN){
                mem->Set(RP, mem->Get(RP) + sizeof(uint32_t));
            }
        }
        return result;
    }

}
