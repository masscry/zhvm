/**
 * @file interpreter.cpp
 * @author marko
 */

#include <cassert>
#include <zhvm.h>
#include <string.h>
#include <iostream>

//#include <gperftools/profiler.h>

namespace zhvm {

    void UnpackCommand(uint32_t cmd, uint32_t *opcode, uint32_t *regs, int32_t *imm) {

        *opcode = cmd & ((1 << 6) - 1);
        regs[CR_DEST] = (cmd >> 6) & ((1 << 4) - 1);
        regs[CR_SRC0] = (cmd >> 10) & ((1 << 4) - 1);
        regs[CR_SRC1] = (cmd >> 14) & ((1 << 4) - 1);
        *imm = (cmd >> 18) & ((1 << 14) - 1);

    }

    struct longcmd {
        uint32_t opc;
        uint32_t regs[CR_TOTAL];
        int32_t imm;
    };

    /**
     * Main interperter function.
     * 
     * icmd passed by value, because significant increase in speed in comparison with passed by reference version.
     *
     * @param mem ZHVM memory
     * @param icmd command to execute
     */
    static int InterpretCommand(zhvm::memory *mem, longcmd icmd) {

        switch (icmd.opc) {
            case OP_HLT:
                return IR_HALT;
            case OP_ADD:
                mem->Set(icmd.regs[CR_DEST], mem->Get(icmd.regs[CR_SRC0]) + (mem->Get(icmd.regs[CR_SRC1]) + icmd.imm));
                break;
            case OP_SUB:
                mem->Set(icmd.regs[CR_DEST], mem->Get(icmd.regs[CR_SRC0]) - (mem->Get(icmd.regs[CR_SRC1]) + icmd.imm));
                break;
            case OP_MUL:
                mem->Set(icmd.regs[CR_DEST], mem->Get(icmd.regs[CR_SRC0]) * (mem->Get(icmd.regs[CR_SRC1]) + icmd.imm));
                break;
            case OP_DIV:
                mem->Set(icmd.regs[CR_DEST], mem->Get(icmd.regs[CR_SRC0]) / (mem->Get(icmd.regs[CR_SRC1]) + icmd.imm));
                break;
            case OP_MOD:
                mem->Set(icmd.regs[CR_DEST], mem->Get(icmd.regs[CR_SRC0]) % (mem->Get(icmd.regs[CR_SRC1]) + icmd.imm));
                break;
            case OP_CMZ:
                if (mem->Get(icmd.regs[CR_SRC0]) == 0) {
                    mem->Set(icmd.regs[CR_DEST], mem->Get(icmd.regs[CR_SRC1]) + icmd.imm);
                }
                break;
            case OP_CMN:
                if (mem->Get(icmd.regs[CR_SRC0]) != 0) {
                    mem->Set(icmd.regs[CR_DEST], mem->Get(icmd.regs[CR_SRC1]) + icmd.imm);
                }
                break;
            case OP_LDB:
                mem->Set(icmd.regs[CR_DEST],
                        mem->GetByte(mem->Get(icmd.regs[CR_SRC0]))
                        + mem->Get(icmd.regs[CR_SRC1]) + icmd.imm
                        );
                break;
            case OP_LDS:
                mem->Set(icmd.regs[CR_DEST],
                        mem->GetShort(mem->Get(icmd.regs[CR_SRC0]))
                        + mem->Get(icmd.regs[CR_SRC1]) + icmd.imm
                        );
                break;
            case OP_LDL:
                mem->Set(icmd.regs[CR_DEST],
                        mem->GetLong(mem->Get(icmd.regs[CR_SRC0]))
                        + mem->Get(icmd.regs[CR_SRC1]) + icmd.imm
                        );
                break;
            case OP_LDQ:
                mem->Set(icmd.regs[CR_DEST],
                        mem->GetQuad(mem->Get(icmd.regs[CR_SRC0]))
                        + mem->Get(icmd.regs[CR_SRC1]) + icmd.imm
                        );
                break;
            case OP_SVB:
                mem->SetByte(mem->Get(icmd.regs[CR_DEST]),
                        mem->Get(icmd.regs[CR_SRC0]) + mem->Get(icmd.regs[CR_SRC1]) + icmd.imm
                        );
                break;
            case OP_SVS:
                mem->SetShort(mem->Get(icmd.regs[CR_DEST]),
                        mem->Get(icmd.regs[CR_SRC0]) + mem->Get(icmd.regs[CR_SRC1]) + icmd.imm
                        );
                break;
            case OP_SVL:
                mem->SetLong(mem->Get(icmd.regs[CR_DEST]),
                        mem->Get(icmd.regs[CR_SRC0]) + mem->Get(icmd.regs[CR_SRC1]) + icmd.imm
                        );
                break;
            case OP_SVQ:
                mem->SetQuad(mem->Get(icmd.regs[CR_DEST]),
                        mem->Get(icmd.regs[CR_SRC0]) + mem->Get(icmd.regs[CR_SRC1]) + icmd.imm
                        );
                break;
            case OP_AND:
                mem->Set(icmd.regs[CR_DEST], mem->Get(icmd.regs[CR_SRC0]) & (mem->Get(icmd.regs[CR_SRC1]) + icmd.imm));
                break;
            case OP_OR:
                mem->Set(icmd.regs[CR_DEST], mem->Get(icmd.regs[CR_SRC0]) | (mem->Get(icmd.regs[CR_SRC1]) + icmd.imm));
                break;
            case OP_XOR:
                mem->Set(icmd.regs[CR_DEST], mem->Get(icmd.regs[CR_SRC0]) ^ (mem->Get(icmd.regs[CR_SRC1]) + icmd.imm));
                break;
            case OP_GR:
                mem->Set(icmd.regs[CR_DEST], mem->Get(icmd.regs[CR_SRC0]) > (mem->Get(icmd.regs[CR_SRC1]) + icmd.imm));
                break;
            case OP_LS:
                mem->Set(icmd.regs[CR_DEST], mem->Get(icmd.regs[CR_SRC0]) < (mem->Get(icmd.regs[CR_SRC1]) + icmd.imm));
                break;
            case OP_GRE:
                mem->Set(icmd.regs[CR_DEST], mem->Get(icmd.regs[CR_SRC0]) >= (mem->Get(icmd.regs[CR_SRC1]) + icmd.imm));
                break;
            case OP_LSE:
                mem->Set(icmd.regs[CR_DEST], mem->Get(icmd.regs[CR_SRC0]) <= (mem->Get(icmd.regs[CR_SRC1]) + icmd.imm));
                break;
            case OP_EQ:
                mem->Set(icmd.regs[CR_DEST], mem->Get(icmd.regs[CR_SRC0]) == (mem->Get(icmd.regs[CR_SRC1]) + icmd.imm));
                break;
            case OP_NEQ:
                mem->Set(icmd.regs[CR_DEST], mem->Get(icmd.regs[CR_SRC0]) != (mem->Get(icmd.regs[CR_SRC1]) + icmd.imm));
                break;
            case OP_CCL:
                return mem->Call(icmd.regs[CR_SRC0] + icmd.regs[CR_SRC1] + icmd.imm);
            case OP_CPY:
            {
                mem->Copy(mem->Get(icmd.regs[CR_DEST]), mem->Get(icmd.regs[CR_SRC0]), mem->Get(icmd.regs[CR_SRC1]) + icmd.imm);
                break;
            }
            case OP_CMP:
            {
                mem->Set(icmd.regs[CR_DEST], mem->Compare(mem->Get(icmd.regs[CR_DEST]), mem->Get(icmd.regs[CR_SRC0]), mem->Get(icmd.regs[CR_SRC1]) + icmd.imm));
                break;
            }
            case OP_ZCL:
            {
                int64_t rs = mem->Get(icmd.regs[CR_SRC0]) - sizeof (uint32_t);
                mem->Set(icmd.regs[CR_SRC0], rs);
                mem->SetLong(rs, mem->Get(icmd.regs[CR_DEST]) + sizeof (uint32_t));
                mem->Set(icmd.regs[CR_DEST], (mem->Get(icmd.regs[CR_SRC1]) + icmd.imm));
                break;
            }
            case OP_RET:
            {
                int64_t rs = mem->Get(icmd.regs[CR_SRC0]);
                int64_t rp = mem->GetLong(rs);
                mem->Set(icmd.regs[CR_SRC0], rs + sizeof (uint32_t));
                mem->Set(icmd.regs[CR_DEST], rp + (mem->Get(icmd.regs[CR_SRC1]) + icmd.imm));
                break;
            }
            case OP_NOP:
                break;
            default:
                return IR_OP_UNKNWN;
        }
        return IR_RUN;
    }

    int Invoke(zhvm::memory *mem, uint32_t icmd) {
        assert(mem);

        mem->DropSet();

        longcmd lcmd;
        UnpackCommand(icmd, &lcmd.opc, lcmd.regs, &lcmd.imm);

        return InterpretCommand(mem, lcmd);
    }

    int Step(zhvm::memory* mem) {
        assert(mem);

        int result = Invoke(mem, mem->GetCode(mem->Get(RP)));
        if ((result == IR_RUN)&&(mem->TestSetRP() == 0)) {
            mem->Set(RP, mem->Get(RP) + sizeof (uint32_t));
        }
        return result;
    }

    int Execute(memory* mem) {
        //        ProfilerStart("EXECUTE.LOG");
        if (mem == 0) {
            return IR_INVALID_POINTER;
        }
        int result = IR_RUN;
        while (result == IR_RUN) {
            result = Step(mem);
        }
        //        ProfilerStop();
        return result;
    }

    /**
     * 
     * Function execute cached commands one by one. Stops if cache ended, or
     * write to RP registered, halted or unknown command found.
     * 
     * @param mem VM memory
     * @param cache commands cache
     * @param blen number of cached commands
     * @return execution state
     */
    static int BurstStep(memory* mem, longcmd* cache, size_t blen) {
        int result = IR_RUN;
        for (size_t i = 0; (i < blen) && (result == IR_RUN); ++i) {
            mem->DropSet();
            result = InterpretCommand(mem, cache[i]);
            if (result == IR_RUN) {
                if (mem->TestSetRP() == 0) {
                    mem->Set(RP, mem->Get(RP) + sizeof (uint32_t));
                } else {
                    return result;
                }
            }
        }
        return result;
    }

    const static size_t ZHVM_PREFETCH_CACHE_SIZE = 16;

    /**
     * 
     * Function fills prefetch commands array.
     * 
     * Current implementation prefethes up to 16 commands. Prefetch stops as
     * maximum size reached, or RP as destination register detected.
     * 
     * CMZ, CMN commands might or might not write to RP. So, this commands are
     * still prefetched in hope that it wont write. That must improve
     * performance in some cases.
     * 
     * @param mem VM memory
     * @param offset commands offset
     * @param cache result prefetched commands cache
     * @param maxsize maximum commands cache
     * @return returns number of actually cached commands
     */
    static size_t FillCache(memory* mem, off_t offset, longcmd* cache, const size_t maxsize) {
        size_t i = 0;
        for (i = 0; i < maxsize; ++i) {
            UnpackCommand(mem->GetCode(offset + i * sizeof (uint32_t)), &cache[i].opc, cache[i].regs, &cache[i].imm);
            if (((cache[i].regs[CR_DEST] == RP) && (cache[i].opc != OP_CMZ) && (cache[i].opc != OP_CMN)) || (cache[i].opc == OP_HLT)) {
                return i + 1;
            }
        }
        return i;
    }

    int ExecutePrefetch(memory* mem) {
        //        ProfilerStart("BURST.LOG");
        if (mem == 0) {
            return IR_INVALID_POINTER;
        }
        int result = IR_RUN;

        longcmd cache[ZHVM_PREFETCH_CACHE_SIZE];

        while (result == IR_RUN) {
            size_t presize = FillCache(mem, mem->Get(RP), cache, ZHVM_PREFETCH_CACHE_SIZE);
            result = BurstStep(mem, cache, presize);
        }
        //        ProfilerStop();
        return result;
    }


}
