/**
 * @file sman.cpp
 * @author marko
 */

#include <cstdlib>
#include <cctype>
#include <cstring>
#include <climits>

#include <zhvm.h>

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

    cchar *ExpectRegister(cchar *text, uint32_t *result) {
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
                    *result = (uint32_t) ((text[1] - '0') + R0);
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

    cchar *ExpectOpcode(cchar *text, uint32_t *result) {
        char buffer[256] = {0};
        text = SkipSpace(text);
        text = ExtractText(text, buffer, 256);
        
        for (uint32_t index = OP_HLT; index<OP_TOTAL; ++index){
            if ((optexts[index]!=0)&&(strcmp(optexts[index], buffer) == 0)){
                *result = index;
                return text;
            }
        }
        *result = OP_UNKNOWN;
        return text;
    }

#define ZHVM_PUT_OPCODE(opcode, result) (result |= (opcode&ZHVM_OPCODE_MASK) << ZHVM_OPCODE_OFFSET)
#define ZHVM_PUT_RGDEST(rgdest, result) (result |= (rgdest&ZHVM_RGDEST_MASK) << ZHVM_RGDEST_OFFSET)
#define ZHVM_PUT_RGSRC0(rgsrc0, result) (result |= (rgsrc0&ZHVM_RGSRC0_MASK) << ZHVM_RGSRC0_OFFSET)
#define ZHVM_PUT_RGSRC1(rgsrc1, result) (result |= (rgsrc1&ZHVM_RGSRC1_MASK) << ZHVM_RGSRC1_OFFSET)
#define ZHVM_PUT_IMVALL(immval, result) (result |= (immval&ZHVM_IMMVAL_MASK) << ZHVM_IMMVAL_OFFSET)

    inline uint32_t PackCommand(uint32_t opcode, const uint32_t *regs, int16_t imm) {
        uint32_t result = 0;
        ZHVM_PUT_OPCODE(opcode, result);
        ZHVM_PUT_RGDEST(regs[CR_DEST], result);
        ZHVM_PUT_RGSRC0(regs[CR_SRC0], result);
        ZHVM_PUT_RGSRC1(regs[CR_SRC1], result);
        ZHVM_PUT_IMVALL(imm, result);
        return result;
    }

    // $dest opcode $s0, $s1, imm
    cchar *Assemble(cchar *text, uint32_t *result) {

        if (text == 0) {
            return 0;
        }

        if (result == 0) {
            return 0;
        }

        text = SkipSpace(text);
        if (strlen(text) == 0){
            *result = 0;
            return text;
        }

        uint32_t regs[CR_TOTAL] = {RZ};
        long int rim = 0;
        uint32_t opcode = OP_HLT;
        char *end = 0;

        text = ExpectRegister(text, &(regs[CR_DEST]));
        switch ((regs[CR_DEST])) {
            case RNOTREG:
                (regs[CR_DEST]) = RZ;
                break;
            case RUNKNWN:
                return 0;
        }

        text = ExpectOpcode(text, &opcode);
        if (opcode == OP_UNKNOWN) {
            return 0;
        }

        text = ExpectRegister(text, &(regs[CR_SRC0]));
        switch ((regs[CR_SRC0])) {
            case RNOTREG:
                (regs[CR_SRC0]) = RZ;
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

        text = ExpectRegister(text, &regs[CR_SRC1]);
        switch (regs[CR_SRC1]) {
            case RNOTREG:
                regs[CR_SRC1] = RZ;
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

    *result = PackCommand(opcode, regs, (int16_t) rim);

        return end;
    }

    cchar* GetRegisterName(uint32_t reg){
        if (reg < RTOTAL){
            return regnames[reg];
        }
        return "$?";
    }

}
