#include "cmplv2.h"
#include "cmplv2.class.h"

#include <cstdarg>
#include <cstdio>
#include <stack>
#include <queue>
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

    cmplv2::cmplv2(FILE* input, memory* mem) : offset(0), context(0), mem(mem) {

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

    uint32_t cmplv2::Offset() const {
        return this->offset;
    }

    int cmplv2::operator()() {
        return this->command();
    }

    enum cmplv2_state {
        CS_START,
        CS_DST,
        CS_SET,
        CS_OPERATOR,
        CS_OPEN,
        CS_ARGS,
        CS_SRC0,
        CS_COMMA_SRC0,
        CS_AFTER_COMMA_SRC0,
        CS_SRC1,
        CS_AFTER_SRC1,
        CS_SIGN,
        CS_NUMBER,
        CS_CLOSE,
        CS_FINISH,
        CS_BAD_END
    };

    struct yydata {
        YYSTYPE tok;
        YYLTYPE loc;
    };

    static bool prepare(yyscan_t scan, std::queue<yydata>& toks) {
        while (toks.size() < 2) {
            yydata temp;

            int next = yylex(&temp.tok, &temp.loc, scan);

            if (next == TT2_EOF) {
                if (toks.empty()) {
                    return false;
                }
                temp.tok.type = TT2_EOF;
            }
            toks.push(temp);
        }
        return true;
    }

    static bool nextToken(yyscan_t scan, std::queue<yydata>& toks) {
        if (toks.empty()) {
            return prepare(scan, toks);
        }
        toks.pop();
        if (toks.back().tok.type != TT2_EOF) {
            return prepare(scan, toks);
        }
        return false;
    }

    enum macrotype {
        MT_BYTE,
        MT_SHORT,
        MT_LONG,
        MT_QUAD,
        MT_LABEL,
        MT_TOTAL
    };

    const char* macrotext[MT_TOTAL + 1] = {
        "byte",
        "short",
        "long",
        "quad",
        ":",
        0
    };

    int getmacro(const char* text) {
        const char** cursor = macrotext;
        int macro = 0;
        while (*cursor != 0) {
            if (strcmp(text, *cursor) == 0) {
                return macro;
            }
            ++macro;
        }
        return MT_TOTAL;
    }

    enum macrostate {
        MS_START,
        MS_NUMBER,
        MS_NAME
    };

    int cmplv2::macro() {
        yydata tok = {0};
        int state = MS_START;

        while (yylex(&tok.tok, &tok.loc, this->context) != TT2_EOF) {
            switch (state) {
                case MS_START:
                    break;
                case MS_NUMBER:
                    break;
                case MS_NAME:
                    break;
            }
        }

        return TT2_EOF;
    }

    int cmplv2::command() {
        std::stack<int> state;
        std::queue<yydata> toks;

        uint32_t regs[3] = {zhvm::RZ, zhvm::RZ, zhvm::RZ};
        uint32_t opcode = zhvm::OP_HLT;
        int16_t imm = 0;
        int16_t signum = 1;

        state.push(CS_START);
        if (!nextToken(this->context, toks)) {
            ErrorMsg(-1, "%s: %s", "FORMAT ERROR", "UNEXPECTED EOF");
            return TT2_ERROR;
        }

        while (!state.empty()) {

            switch (state.top()) {
                case CS_START:
                {
                    switch (toks.front().tok.type) {
                        case TT2_REG:
                            state.push(CS_DST);
                            break;
                        case TT2_WORD:
                            regs[0] = zhvm::RZ;
                            state.push(CS_OPERATOR);
                            break;
                        case TT2_EOF:
                            state.pop();
                            break;
                        case TT2_MACRO:
                            if (this->macro() == TT2_ERROR) {
                                state.push(CS_BAD_END);
                            }
                            break;
                        default:
                            ErrorMsg(toks.front().loc, "%s: %s", "SYNTAX ERROR", "REG, OPERATOR or MACRO expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_DST:
                {
                    switch (toks.front().tok.type) {
                        case TT2_REG:
                            regs[0] = toks.front().tok.reg.val;
                            state.top() = CS_SET;
                            if (!nextToken(this->context, toks)) {
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "UNEXPECTED EOF");
                                state.push(CS_BAD_END);
                            }
                            break;
                        default:
                            ErrorMsg(toks.front().loc, "%s: %s", "SYNTAX ERROR", "REG expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_SET:
                {
                    switch (toks.front().tok.type) {
                        case TT2_SET:
                            state.top() = CS_OPERATOR;
                            if (!nextToken(this->context, toks)) {
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "UNEXPECTED EOF");
                                state.push(CS_BAD_END);
                            }
                            break;
                        default:
                            ErrorMsg(toks.front().loc, "%s: %s", "SYNTAX ERROR", "= expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_OPERATOR:
                {
                    switch (toks.front().tok.type) {
                        case TT2_WORD:
                            opcode = zhvm::GetOpcode(toks.front().tok.opr.val);
                            state.top() = CS_OPEN;
                            if (!nextToken(this->context, toks)) {
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "UNEXPECTED EOF");
                                state.push(CS_BAD_END);
                            }
                            break;
                        default:
                            ErrorMsg(toks.front().loc, "%s: %s", "SYNTAX ERROR", "OPERATOR expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_OPEN:
                {
                    switch (toks.front().tok.type) {
                        case TT2_OPEN:
                            state.top() = CS_ARGS;
                            if (!nextToken(this->context, toks)) {
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "UNEXPECTED EOF");
                                state.push(CS_BAD_END);
                            }
                            break;
                        default:
                            ErrorMsg(toks.front().loc, "%s: %s", "SYNTAX ERROR", "[ expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_ARGS:
                {
                    switch (toks.front().tok.type) {
                        case TT2_REG:
                            state.top() = CS_SRC0;
                            break;
                        case TT2_COMMA:
                            regs[1] = zhvm::RZ;
                            state.top() = CS_COMMA_SRC0;
                            break;
                        case TT2_CLOSE:
                            regs[1] = zhvm::RZ;
                            regs[2] = zhvm::RZ;
                            imm = 0;
                            state.top() = CS_CLOSE;
                            break;
                        default:
                            ErrorMsg(toks.front().loc, "%s: %s", "SYNTAX ERROR", "REG, COMMA, SIGN or NUMBER, or ] expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_SRC0:
                {
                    switch (toks.front().tok.type) {
                        case TT2_REG:
                            regs[1] = toks.front().tok.reg.val;
                            state.top() = CS_COMMA_SRC0;
                            if (!nextToken(this->context, toks)) {
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "UNEXPECTED EOF");
                                state.push(CS_BAD_END);
                            }
                            break;
                        default:
                            ErrorMsg(toks.front().loc, "%s: %s", "SYNTAX ERROR", "REG expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_COMMA_SRC0:
                {
                    switch (toks.front().tok.type) {
                        case TT2_COMMA:
                            state.top() = CS_AFTER_COMMA_SRC0;
                            if (!nextToken(this->context, toks)) {
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "UNEXPECTED EOF");
                                state.push(CS_BAD_END);
                            }
                            break;
                        default:
                            ErrorMsg(toks.front().loc, "%s: %s", "SYNTAX ERROR", ", expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_AFTER_COMMA_SRC0:
                {
                    switch (toks.front().tok.type) {
                        case TT2_REG:
                            state.top() = CS_SRC1;
                            break;
                        case TT2_SIGN_MINUS:
                        case TT2_SIGN_PLUS:
                            state.top() = CS_SIGN;
                            break;
                        case TT2_NUMBER:
                            state.top() = CS_NUMBER;
                            break;
                        case TT2_CLOSE:
                            regs[2] = zhvm::RZ;
                            imm = 0;
                            state.top() = CS_CLOSE;
                            break;
                        default:
                            ErrorMsg(toks.front().loc, "%s: %s", "SYNTAX ERROR", "REG, SIGN or NUMBER, or ] expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_SRC1:
                {
                    switch (toks.front().tok.type) {
                        case TT2_REG:
                            regs[1] = toks.front().tok.reg.val;
                            state.top() = CS_AFTER_SRC1;
                            if (!nextToken(this->context, toks)) {
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "UNEXPECTED EOF");
                                state.push(CS_BAD_END);
                            }
                            break;
                        default:
                            ErrorMsg(toks.front().loc, "%s: %s", "SYNTAX ERROR", "REG expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_AFTER_SRC1:
                {
                    switch (toks.front().tok.type) {
                        case TT2_SIGN_MINUS:
                        case TT2_SIGN_PLUS:
                            state.top() = CS_SIGN;
                            break;
                        case TT2_NUMBER:
                            state.top() = CS_NUMBER;
                            break;
                        case TT2_CLOSE:
                            imm = 0;
                            state.top() = CS_CLOSE;
                            break;
                        default:
                            ErrorMsg(toks.front().loc, "%s: %s", "SYNTAX ERROR", "SIGN or NUMBER, or ] expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_SIGN:
                {
                    switch (toks.front().tok.type) {
                        case TT2_SIGN_MINUS:
                            signum = -1;
                            state.top() = CS_NUMBER;
                            if (!nextToken(this->context, toks)) {
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "UNEXPECTED EOF");
                                state.push(CS_BAD_END);
                            }
                            break;
                        case TT2_SIGN_PLUS:
                            signum = 1;
                            state.top() = CS_NUMBER;
                            if (!nextToken(this->context, toks)) {
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "UNEXPECTED EOF");
                                state.push(CS_BAD_END);
                            }
                            break;
                        default:
                            ErrorMsg(toks.front().loc, "%s: %s", "SYNTAX ERROR", "SIGN expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_NUMBER:
                {
                    if ((toks.front().tok.num.val > ZHVM_IMMVAL_MAX) || (toks.front().tok.num.val < ZHVM_IMMVAL_MIN)) {
                        ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "14-BIT NUMBER EXPECTED");
                        state.top() = CS_BAD_END;
                        break;
                    }
                    imm = toks.front().tok.num.val & 0xFFFF;
                    state.top() = CS_CLOSE;
                    if (!nextToken(this->context, toks)) {
                        ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "UNEXPECTED EOF");
                        state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_CLOSE:
                {
                    switch (toks.front().tok.type) {
                        case TT2_CLOSE:
                            state.top() = CS_FINISH;
                            nextToken(this->context, toks);
                            break;
                        default:
                            ErrorMsg(toks.front().loc, "%s: %s", "SYNTAX ERROR", "] expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_FINISH:
                {
                    uint32_t cmd = zhvm::PackCommand(opcode, regs, imm * signum);
                    mem->SetLong(offset, (uint32_t) cmd);
                    offset += sizeof (uint32_t);

                    LogMsg("0x%08x", cmd);

                    regs[0] = zhvm::RZ;
                    regs[1] = zhvm::RZ;
                    regs[2] = zhvm::RZ;
                    opcode = zhvm::OP_HLT;
                    imm = 0;
                    signum = 1;

                    state.pop();
                    break;
                }
                case CS_BAD_END:
                    return TT2_ERROR;
                default:
                    ErrorMsg(toks.front().loc, "%s: %s", "PARSER ERROR", "UNKNOWN STATE");
                    state.push(CS_BAD_END);
            }

        }
        if (state.empty()) {
            return TT2_EOF;
        }
        return TT2_ERROR;
    }


}