//
// Created by timur on 12.07.16.
//

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdint>
#include <cctype>
#include <climits>
#include <iostream>
#include "zhvm.h"

namespace zhvm {

    const char *SkipSpace(const char *text) {
        while ((*text!=0) && isspace(*text)) {
            text++;
        }
        return text;
    }

    const char *ExpectSymbol(char smb, const char *text) {
        text = SkipSpace(text);
        if (*text == smb) {
            return text + 1;
        }
        if (*text == 0){
            return text;
        }
        return 0;
    }

    const char *ExpectRegister(const char *text, int32_t *result) {
        text = SkipSpace(text);
        if (text[0] == '$') {
            switch (text[1]) {
                case 'z':
                case 'Z':
                    *result = RZ;
                    break;

                case 'a':
                case 'A':
                    *result = RA;
                    break;

                case 'b':
                case 'B':
                    *result = RB;
                    break;

                case 'c':
                case 'C':
                    *result = RC;
                    break;

                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                    *result = (text[1] - '0') + R0;
                    break;

                case 's':
                case 'S':
                    *result = RS;
                    break;

                case 'd':
                case 'D':
                    *result = RD;
                    break;

                case 'p':
                case 'P':
                    *result = RP;
                    break;

                default:
                    *result = RUNKNWN;
            }
            return text + 2;
        }

        *result = RNOTREG;
        return text;
    }

    const char *optexts[OP_TOTAL+1] = {
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
            0
    };

    const char *ExtractText(const char *text, char *result, size_t maxresult) {
        text = SkipSpace(text);
        while ((*text != 0)&&(!isspace(*text)) && (maxresult > 0)) {
            *result++ = *text++;
            --maxresult;
        }
        *result = 0;
        return text;
    }

    const char *ExpectOpcode(const char *text, int32_t *result) {
        char buffer[256] = {0};
        text = SkipSpace(text);
        text = ExtractText(text, buffer, 256);

        int32_t index = OP_HALT;

        const char **ptr = optexts;
        while (*ptr != 0) {
            if (strcmp(*ptr, buffer) == 0) {
                *result = index;
                return text;
            }
            ++index;
            ++ptr;
        }
        *result = OP_UNKNOWN;
        return text;
    }


    // $dest opcode $s0, $s1, imm
    const char *Assemble(const char *text, cmd *result) {
        int32_t rd = RZ;
        int32_t rs0 = RZ;
        int32_t rs1 = RZ;
        int64_t rim = 0;
        int32_t opcode = OP_HALT;
        char *end = 0;

        text = ExpectRegister(text, &rd);
        switch (rd) {
            case RNOTREG:
                rd = RZ;
                break;
            case RUNKNWN:
                return 0;
        }

        text = ExpectOpcode(text, &opcode);
        if (opcode == OP_UNKNOWN) {
            return 0;
        }
        
        text = ExpectRegister(text, &rs0);
        switch (rs0) {
            case RNOTREG:
                rs0 = RZ;
                break;
            case RUNKNWN:
                return 0;
        }

        text = ExpectSymbol(',', text);
        if (text == 0) {
            return 0;
        }
        
        if (*text == 0){
            end = const_cast<char*>(text);
            goto RETURN_RESULT;
        }

        text = ExpectRegister(text, &rs1);
        switch (rs1) {
            case RNOTREG:
                rs1 = RZ;
                break;
            case RUNKNWN:
            case R0:
            case R1:
            case R2:
            case R3:
            case R4:
            case R5:
            case R6:
            case R7:
            case R8:
            case RS:
            case RD:
            case RP:
                return 0;
        }

        text = ExpectSymbol(',', text);
        if (text == 0) {
            return 0;
        }
        if (*text == 0){
            end = const_cast<char*>(text);
            goto RETURN_RESULT;
        }

        end = 0;
        rim = strtol(text, &end, 10);

        if ((rim > SHRT_MAX)||(rim < SHRT_MIN)){
            return 0;
        }

    RETURN_RESULT:
    
        result->opc = opcode;
        result->dr = rd;
        result->ds0 = rs0;
        result->ds1 = rs1;
        result->im = rim;

        return end;
    }

    uint32_t AsBytes(cmd command) {
        return *reinterpret_cast<uint32_t *>(&command);
    }

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
            ucmd icmd;
            icmd.i32 = mem->GetLong(mem->Get(RP));
            result = InterpretCommand(mem, icmd.c32);
            if (result == IR_RUN){
                mem->Set(RP, mem->Get(RP)+sizeof(cmd));
            }
        }
        return result;
    }

    const char* GetRegisterName(uint32_t reg){
        if (reg < RTOTAL){
            static const char* names[] = {
                "RZ",
                "RA",
                "RB",
                "RC",
                "R0",
                "R1",
                "R2",
                "R3",
                "R4",
                "R5",
                "R6",
                "R7",
                "R8",
                "RS",
                "RD",
                "RP"
            };
            return names[reg];
        }
        return "RUNKNW";
    }




}
