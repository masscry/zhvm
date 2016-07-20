/**
 * @file sman.cpp
 * @author marko
 */

#include <cstdlib>
#include <cctype>
#include <cstring>
#include <climits>

#include "zhvm.h"
#include "sman.h"

namespace {

    using namespace zhvm;

    cchar* regnames[RTOTAL+1] = {
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

    cchar *optexts[OP_TOTAL+1] = {
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

}


namespace zhvm {

    cchar *SkipSpace(cchar *text) {
        while ((*text!=0) && isspace(*text)) {
            text++;
        }
        return text;
    }

    cchar *ExpectSymbol(char smb, cchar *text) {
        text = SkipSpace(text);
        if (*text == smb) {
            return text + 1;
        }
        if (*text == 0){
            return text;
        }
        return 0;
    }

    cchar *ExpectRegister(cchar *text, int32_t *result) {
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

    cchar *ExtractText(cchar *text, char *result, size_t maxresult) {
        text = SkipSpace(text);
        while ((*text != 0)&&(!isspace(*text)) && (maxresult > 0)) {
            *result++ = *text++;
            --maxresult;
        }
        *result = 0;
        return text;
    }

    cchar *ExpectOpcode(cchar *text, int32_t *result) {
        char buffer[256] = {0};
        text = SkipSpace(text);
        text = ExtractText(text, buffer, 256);

        int32_t index = OP_HALT;

        cchar **ptr = optexts;
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
    cchar *Assemble(cchar *text, cmd *result) {
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

    cchar* GetRegisterName(uint32_t reg){
        if (reg < RTOTAL){
            return regnames[reg];
        }
        return "$?";
    }

}
