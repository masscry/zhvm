#include "cmplv2.h"
#include "cmplv2.class.h"

#include <cstdarg>
#include <cstdio>
#include <stack>
#include <climits>
#include <stdexcept>

namespace zhvm {

    void ErrorMsg(location loc, const char* format, ...) {

        char buffer[256] = "";

        va_list va;
        va_start(va, format);
        vsnprintf(buffer, 256, format, va);
        va_end(va);

        fprintf(stderr, "%d: %s\n", loc, buffer);
    }

    void LogMsg(const char* format, ...) {
        char buffer[256] = "";

        va_list va;
        va_start(va, format);
        vsnprintf(buffer, 256, format, va);
        va_end(va);

        fprintf(stderr, "LOG: %s\n", buffer);
    }

    cmplv2::cmplv2(FILE* input, memory* mem) : context(0), mem(mem) {

        if (this->mem == 0) {
            throw std::runtime_error("Invalid memory pointer");
        }

        if (input == 0) {
            throw std::runtime_error("Invalid input file");
        }

        yylex_init(&this->context);
        yyset_in(input, this->context);
    }

    cmplv2::~cmplv2() {
        yylex_destroy(this->context);
        this->context = 0;
    }

    int cmplv2::operator()() {
        return this->invoke();
    }

    enum cmplv2_state {
        CS_START,
        CS_SET,
        CS_OPERATOR,
        CS_OPEN,
        CS_ARGS,
        CS_SRC0,
        CS_COMMA,
        CS_SRC1,
        CS_NUMBER,
        CS_CLOSE,
        CS_FINISH,
        CS_BAD_END
    };

    int cmplv2::invoke() {

        YYSTYPE ctok; // current token
        YYLTYPE cloc; // current location

        std::stack<int> state;

        uint32_t regs[3] = {zhvm::RZ, zhvm::RZ, zhvm::RZ};
        uint32_t opcode = zhvm::OP_HLT;
        int16_t imm = 0;
        uint32_t offset = 0;
        int16_t signum = 1;

        state.push(CS_START);

#define NEXT_TOKEN yylex(&ctok, &cloc, this->context);

        NEXT_TOKEN;
        while (ctok.type != TT2_EOF) {

            switch (state.top()) {

                case CS_START:
                {
                    switch (ctok.type) {
                        case TT2_LOREG:
                        case TT2_HIREG:
                            regs[0] = ctok.reg.val;
                            state.top() = CS_SET;
                            NEXT_TOKEN;
                            break;
                        case TT2_OPERATOR:
                            regs[0] = zhvm::RZ;
                            state.top() = CS_OPERATOR;
                            break;
                        default:
                            ErrorMsg(cloc, "%s: %s", "SYNTAX ERROR", "LOREG, HIREG or OPERATOR expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_SET:
                {
                    switch (ctok.type) {
                        case TT2_SET:
                            state.top() = CS_OPERATOR;
                            break;
                        default:
                            ErrorMsg(cloc, "%s: %s", "SYNTAX ERROR", "= expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_OPERATOR:
                {
                    switch (ctok.type) {
                        case TT2_OPERATOR:
                            opcode = zhvm::GetOpcode(ctok.opr.val);
                            state.top() = CS_OPEN;
                            NEXT_TOKEN;
                            break;
                        default:
                            ErrorMsg(cloc, "%s: %s", "SYNTAX ERROR", "OPERATOR expected");
                            state.push(CS_BAD_END);
                    }
                }
                case CS_OPEN:
                {
                    switch (ctok.type) {
                        case TT2_OPEN:
                            state.top() = CS_ARGS;
                            NEXT_TOKEN;
                            break;
                        default:
                            ErrorMsg(cloc, "%s: %s", "SYNTAX ERROR", "[ expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_ARGS:
                {
                    switch (ctok.type) {
                        case TT2_LOREG:
                        case TT2_HIREG:
                            state.top() = CS_SRC0;
                            break;
                        case TT2_COMMA:
                            regs[1] = zhvm::RZ;
                            state.top() = CS_COMMA;
                            break;
                        case TT2_NUMBER:
                            regs[1] = zhvm::RZ;
                            regs[2] = zhvm::RZ;
                            state.top() = CS_NUMBER;
                            break;
                        case TT2_CLOSE:
                            regs[1] = zhvm::RZ;
                            regs[2] = zhvm::RZ;
                            imm = 0;
                            state.top() = CS_CLOSE;
                            break;
                        default:
                            ErrorMsg(cloc, "%s: %s", "SYNTAX ERROR", "LOREG, HIREG, SIGN, COMMA, NUMBER,  or ] expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_SRC0:
                {
                    switch (ctok.type) {
                        case TT2_LOREG:
                        case TT2_HIREG:
                            regs[1] = ctok.reg.val;
                            state.top() = CS_COMMA;
                            NEXT_TOKEN;
                            break;
                        default:
                            ErrorMsg(cloc, "%s: %s", "SYNTAX ERROR", "LOREG or HIREG expected");
                            state.push(CS_BAD_END);
                    }
                }
                case CS_SRC1:
                {
                    switch (ctok.type) {
                        case TT2_LOREG:
                            regs[1] = ctok.reg.val;
                            state.top() = CS_COMMA;
                            NEXT_TOKEN;
                            break;
                        default:
                            ErrorMsg(cloc, "%s: %s", "SYNTAX ERROR", "LOREG or HIREG expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_NUMBER:
                {
                    if ((ctok.num.val > SHRT_MAX) || (ctok.num.val < SHRT_MIN)) {
                        ErrorMsg(cloc, "%s: %s", "FORMAT ERROR", "16-BIT NUMBER EXPECTED");
                        state.top() = CS_BAD_END;
                        break;
                    }
                    imm = ctok.num.val & 0xFFFF;
                    state.top() = CS_CLOSE;
                    NEXT_TOKEN;
                    break;
                }
                case CS_CLOSE:
                {
                    switch (ctok.type) {
                        case TT2_CLOSE:
                            state.top() = CS_FINISH;
                            NEXT_TOKEN;
                            break;
                        default:
                            ErrorMsg(cloc, "%s: %s", "SYNTAX ERROR", "] expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_FINISH:
                {
                    uint32_t cmd = zhvm::PackCommand(opcode, regs, imm);
                    mem->SetLong(offset, (uint32_t) cmd);
                    offset += sizeof (uint32_t);

                    LogMsg("0x%08x", cmd);

                    regs[0] = zhvm::RZ;
                    regs[1] = zhvm::RZ;
                    regs[2] = zhvm::RZ;
                    opcode = zhvm::OP_HLT;
                    imm = 0;
                    signum = 1;

                    state.top() = CS_START;
                    break;
                }
                case CS_BAD_END:
                    return TT2_ERROR;
                default:
                    ErrorMsg(cloc, "%s: %s", "PARSER ERROR", "UNKNOWN STATE");
                    state.push(CS_BAD_END);
            }

        }

        if ((ctok.type == TT2_EOF)&&(state.top() == CS_START)) {
            return TT2_EOF;
        }
        return TT2_ERROR;
    }




}