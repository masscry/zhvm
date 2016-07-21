/**
 * @file interpreter.cpp
 * @author marko
 */

#include <zhvm.h>

namespace zhvm {

    static int InterpretCommand(memory* mem, cmd icmd){
        switch (icmd.opc){
            case OP_HALT:
                return IR_HALT;
            case OP_ADD:
                mem->Set(icmd.dr, mem->Get(icmd.ds0)+(mem->Get(icmd.ds1)+icmd.im));
                break;
            case OP_SUB:
                mem->Set(icmd.dr, mem->Get(icmd.ds0)-(mem->Get(icmd.ds1)+icmd.im));
                break;
            case OP_MUL:
                mem->Set(icmd.dr, mem->Get(icmd.ds0)*(mem->Get(icmd.ds1)+icmd.im));
                break;
            case OP_DIV:
                mem->Set(icmd.dr, mem->Get(icmd.ds0)/(mem->Get(icmd.ds1)+icmd.im));
                break;
            case OP_MOD:
                mem->Set(icmd.dr, mem->Get(icmd.ds0)%(mem->Get(icmd.ds1)+icmd.im));
                break;
            case OP_CMZ:
                if (mem->Get(icmd.ds0) == 0){
                    mem->Set(icmd.dr, mem->Get(icmd.ds1)+icmd.im);
                }
                break;
            case OP_CMN:
                if (mem->Get(icmd.ds0) != 0){
                    mem->Set(icmd.dr, mem->Get(icmd.ds1)+icmd.im);
                }
                break;
            case OP_LDB:
                mem->Set(
                        icmd.dr,
                        mem->GetByte(
                                mem->Get(icmd.ds0)
                                +mem->Get(icmd.ds1)
                                +icmd.im
                        )
                );
                break;
            case OP_LDS:
                mem->Set(
                        icmd.dr,
                        mem->GetShort(
                                mem->Get(icmd.ds0)
                                +mem->Get(icmd.ds1)
                                +icmd.im
                        )
                );
                break;
            case OP_LDL:
                mem->Set(
                        icmd.dr,
                        mem->GetLong(
                                mem->Get(icmd.ds0)
                                +mem->Get(icmd.ds1)
                                +icmd.im
                        )
                );
                break;
            case OP_LDQ:
                mem->Set(
                        icmd.dr,
                        mem->GetQuad(
                                mem->Get(icmd.ds0)
                                +mem->Get(icmd.ds1)
                                +icmd.im
                        )
                );
                break;
            case OP_SVB:
                mem->SetByte(
                        mem->Get(icmd.dr)
                        +mem->Get(icmd.ds1)
                        +icmd.im,
                        mem->Get(icmd.ds0)
                );
                break;
            case OP_SVS:
                mem->SetShort(
                        mem->Get(icmd.dr)
                        +mem->Get(icmd.ds1)
                        +icmd.im,
                        mem->Get(icmd.ds0)
                );
                break;
            case OP_SVL:
                mem->SetLong(
                        mem->Get(icmd.dr)
                        +mem->Get(icmd.ds1)
                        +icmd.im,
                        mem->Get(icmd.ds0)
                );
                break;
            case OP_SVQ:
                mem->SetQuad(
                        mem->Get(icmd.dr)
                        +mem->Get(icmd.ds1)
                        +icmd.im,
                        mem->Get(icmd.ds0)
                );
                break;
            case OP_AND:
                mem->Set(icmd.dr, mem->Get(icmd.ds0)&(mem->Get(icmd.ds1)+icmd.im));
                break;
            case OP_OR:
                mem->Set(icmd.dr, mem->Get(icmd.ds0)|(mem->Get(icmd.ds1)+icmd.im));
                break;
            case OP_XOR:
                mem->Set(icmd.dr, mem->Get(icmd.ds0)^(mem->Get(icmd.ds1)+icmd.im));
                break;
            default:
                return IR_OP_UNKNWN;
        }
        return IR_RUN;
    }
    
    int Invoke(memory* mem, cmd icmd){
        int result = InterpretCommand(mem, icmd);
        if (result == IR_RUN){
            mem->SetLong(mem->Get(RP), *reinterpret_cast<int32_t*>(&icmd) );
            mem->Set(RP, mem->Get(RP)+sizeof(cmd));
        }
        return result;
    }
    
    int Execute(memory* mem){
        int result = IR_RUN;
        while (result == IR_RUN){
            result = InterpretCommand(mem, AsCommand(mem->GetLong(mem->Get(RP))));
            if (result == IR_RUN){
                mem->Set(RP, mem->Get(RP)+sizeof(cmd));
            }
        }
        return result;
    }

}
