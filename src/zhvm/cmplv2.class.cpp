#include <zhvm.h>
#include <cmplv2.gen.h>

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

    cmplv2::cmplv2(const char* input, memory* mem) : labels(), fixes(), code_offset(0), data_offset(0), cur_offset(0), context(0), bs(0), mem(mem) {
        if (this->mem == 0) {
            throw std::runtime_error("Invalid memory pointer");
        }

        if (input == 0) {
            throw std::runtime_error("Invalid input file");
        }

        cmplv2lex_init(&this->context);
        this->bs = cmplv2_scan_string(input, this->context);
        this->code_offset = mem->Get(zhvm::RP);
        this->data_offset = mem->Get(zhvm::RD);
        this->cur_offset = &this->code_offset;
    }

    cmplv2::cmplv2(FILE* input, memory* mem) : labels(), fixes(), code_offset(0), data_offset(0), cur_offset(0), context(0), bs(0), mem(mem) {

        if (this->mem == 0) {
            throw std::runtime_error("Invalid memory pointer");
        }

        if (input == 0) {
            throw std::runtime_error("Invalid input file");
        }

        cmplv2lex_init(&this->context);
        cmplv2set_in(input, this->context);
        this->code_offset = mem->Get(zhvm::RP);
        this->data_offset = mem->Get(zhvm::RD);
        this->cur_offset = &this->code_offset;
    }

    cmplv2::~cmplv2() {
        if (this->bs != 0) {
            cmplv2_delete_buffer(this->bs, this->context);
        }
        cmplv2lex_destroy(this->context);
        this->context = 0;
    }

    uint32_t cmplv2::CodeOffset() const {
        return this->code_offset;
    }

    uint32_t cmplv2::DataOffset() const {
        return this->data_offset;
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
        CS_AT,
        CS_CLOSE,
        CS_FINISH,
        CS_BAD_END
    };

    const char* statetext[] = {
        "START",
        "DST",
        "SET",
        "OPERATOR",
        "OPEN",
        "ARGS",
        "SRC0",
        "COMMA SRC0",
        "AFTER COMMA SRC0",
        "SRC1",
        "AFTER SRC1",
        "SIGN",
        "NUMBER",
        "AT",
        "CLOSE",
        "FINISH",
        "BAD END",
        0
    };

    static bool prepare(yyscan_t scan, std::queue<yydata>& toks) {
        while (toks.size() < 2) {
            yydata temp;

            int next = cmplv2lex(&temp.tok, &temp.loc, scan);

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
        MT_CODE,
        MT_DATA,
        MT_TOTAL
    };

    enum macrostate {
        MS_START,
        MS_NUMBER,
        MS_LABEL,
        MS_REGISTER
    };

    enum labeltype {
        LT_CODE,
        LT_DATA,
        LT_LABEL,
    };

    static labeltype identifyLabel(const char* lb) {
        if (strcmp(lb, "code") == 0) {
            return LT_CODE;
        }
        if (strcmp(lb, "data") == 0) {
            return LT_DATA;
        }
        return LT_LABEL;
    }

    int cmplv2::macro(std::queue<yydata>* toks) {
        std::queue<yydata>& tks = *toks;
        int state = MS_START;

        while (tks.front().tok.type != TT2_EOF) {
            switch (state) {
                case MS_START:
                {
                    auto& tksfront = tks.front();
                    switch (tksfront.tok.type) {
                        case TT2_NUMBER_BYTE:
                        case TT2_NUMBER_SHORT:
                        case TT2_NUMBER_LONG:
                        case TT2_NUMBER_QUAD:
                            state = MS_NUMBER;
                            break;
                        case TT2_WORD:
                            state = MS_LABEL;
                            break;
                        case TT2_REG:
                            state = MS_REGISTER;
                            break;
                        default:
                            ErrorMsg(tksfront.loc, "%s: %s", "FORMAT ERROR", "NUMBER EXPECTED");
                            return TT2_ERROR;
                    }
                    break;
                }
                case MS_NUMBER:
                {
                    auto& tksfront = tks.front();
                    switch (tksfront.tok.type) {
                        case TT2_NUMBER_BYTE:
                            this->mem->SetByte(this->data_offset, tksfront.tok.num);
                            this->data_offset += sizeof (int8_t);
                            LogMsg("0x%04x: 0x%02x", this->data_offset - (uint32_t)sizeof (int8_t), tksfront.tok.num);
                            if (!nextToken(this->context, tks)) {
                                ErrorMsg(tksfront.loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
                                return TT2_ERROR;
                            }
                            return TT2_EOF;
                        case TT2_NUMBER_SHORT:
                            this->mem->SetShort(this->data_offset, tksfront.tok.num);
                            this->data_offset += sizeof (int16_t);
                            LogMsg("0x%04x: 0x%04x", this->data_offset - (uint32_t)sizeof (int16_t), tksfront.tok.num);
                            if (!nextToken(this->context, tks)) {
                                ErrorMsg(tksfront.loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
                                return TT2_ERROR;
                            }
                            return TT2_EOF;
                        case TT2_NUMBER_LONG:
                            this->mem->SetLong(this->data_offset, tksfront.tok.num);
                            this->data_offset += sizeof (int32_t);
                            LogMsg("0x%04x: 0x%08x", this->data_offset - (uint32_t)sizeof (int32_t), tksfront.tok.num);
                            if (!nextToken(this->context, tks)) {
                                ErrorMsg(tksfront.loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
                                return TT2_ERROR;
                            }
                            return TT2_EOF;
                        case TT2_NUMBER_QUAD:
                            this->mem->SetQuad(this->data_offset, tksfront.tok.num);
                            this->data_offset += sizeof (int64_t);
                            LogMsg("0x%04x: 0x%016x", this->data_offset - (uint32_t)sizeof (int64_t), tksfront.tok.num);
                            if (!nextToken(this->context, tks)) {
                                ErrorMsg(tksfront.loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
                                return TT2_ERROR;
                            }
                            return TT2_EOF;
                        default:
                            ErrorMsg(tksfront.loc, "%s: %s", "FORMAT ERROR", "NUMBER EXPECTED");
                            return TT2_ERROR;
                    }
                }
                case MS_LABEL:
                {
                    auto& tksfront = tks.front();
                    switch (tksfront.tok.type) {
                        case TT2_WORD:
                        {
                            switch (identifyLabel(tksfront.tok.opr.c_str())) {
                                case LT_CODE:
                                    this->cur_offset = &this->code_offset;
                                    LogMsg("%s: 0x%04x", "CODE SECTION", this->code_offset);
                                    if (!nextToken(this->context, tks)) {
                                        ErrorMsg(tksfront.loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
                                        return TT2_ERROR;
                                    }
                                    return TT2_EOF;
                                case LT_DATA:
                                    this->cur_offset = &this->data_offset;
                                    LogMsg("%s: 0x%04x", "DATA SECTION", this->data_offset);
                                    if (!nextToken(this->context, tks)) {
                                        ErrorMsg(tksfront.loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
                                        return TT2_ERROR;
                                    }
                                    return TT2_EOF;
                                case LT_LABEL:
                                    auto oldlb = this->labels.find(tksfront.tok.opr);
                                    if (oldlb != this->labels.end()) {
                                        ErrorMsg(tksfront.loc, "%s: %s %s", "LABEL ERROR", tksfront.tok.opr.c_str(), " is already defined");
                                        return TT2_ERROR;
                                    }
                                    this->labels[tksfront.tok.opr] = *this->cur_offset;
                                    LogMsg("%s: 0x%04x", tksfront.tok.opr.c_str(), *this->cur_offset);
                                    if (!nextToken(this->context, tks)) {
                                        ErrorMsg(tksfront.loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
                                        return TT2_ERROR;
                                    }
                                    return TT2_EOF;
                            }
                        }
                        default:
                            ErrorMsg(tksfront.loc, "%s: %s", "FORMAT ERROR", "WORD EXPECTED");
                            return TT2_ERROR;
                    }
                    break;
                }
                case MS_REGISTER:
                {
                    if (tks.front().tok.type != TT2_REG) {
                        ErrorMsg(tks.front().loc, "%s: %s", "FORMAT ERROR", "REGISTER EXPECTED");
                        return TT2_ERROR;
                    }

                    uint32_t reg = tks.front().tok.reg;
                    if (!nextToken(this->context, tks)) {
                        ErrorMsg(tks.front().loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
                        return TT2_ERROR;
                    }
                    yydata& tksfront = tks.front();
                    switch (tksfront.tok.type) {
                        case TT2_NUMBER_BYTE:
                        case TT2_NUMBER_SHORT:
                        case TT2_NUMBER_LONG:
                        case TT2_NUMBER_QUAD:
                        {
                            this->mem->Set(reg, tksfront.tok.num);
                            LogMsg("%s := 0x%016x", GetRegisterName(reg), tksfront.tok.num);
                            if (!nextToken(this->context, tks)) {
                                ErrorMsg(tksfront.loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
                                return TT2_ERROR;
                            }
                            return TT2_EOF;
                        }
                        case TT2_WORD:
                        {
                            switch (identifyLabel(tksfront.tok.opr.c_str())) {
                                case LT_LABEL:
                                {
                                    auto lb = this->labels.find(tksfront.tok.opr);
                                    if (lb == this->labels.end()) {
                                        ErrorMsg(tksfront.loc, "%s: %s", "FORMAT ERROR", "Already defined label expected");
                                        return TT2_ERROR;
                                    } else { // Already declared
                                        this->mem->Set(reg, lb->second);
                                    }

                                    LogMsg("%s := 0x%016x [%s]", GetRegisterName(reg), lb->second, lb->first.c_str());
                                    if (!nextToken(this->context, tks)) {
                                        ErrorMsg(tksfront.loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
                                        return TT2_ERROR;
                                    }
                                    return TT2_EOF;
                                }
                                default:
                                    ErrorMsg(tksfront.loc, "%s: %s", "FORMAT ERROR", "VALID LABEL EXPECTED");
                                    return TT2_ERROR;
                            }
                        }
                        default:
                            ErrorMsg(tksfront.loc, "%s: %s", "FORMAT ERROR", "NUMBER OR WORD EXPECTED");
                            return TT2_ERROR;
                    }
                    break;
                }
            }
        }
        return TT2_EOF;
    }

    int cmplv2::command() {
        std::stack<cmplv2_state> state;
        std::queue<yydata> toks;

        uint32_t regs[3] = {zhvm::RZ, zhvm::RZ, zhvm::RZ};
        uint32_t opcode = zhvm::OP_HLT;
        int32_t imm = 0;
        int16_t signum = 1;

        state.push(CS_START);
        if (!nextToken(this->context, toks)) {
            ErrorMsg(-1, "%s: %s", "FORMAT ERROR", "unexpected eof");
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

                            if (!nextToken(this->context, toks)) {
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
                                state.push(CS_BAD_END);
                                return TT2_ERROR;
                            }

                            if (this->macro(&toks) == TT2_ERROR) {
                                state.push(CS_BAD_END);
                            }

                            state.top() = CS_START;

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
                            regs[CR_DEST] = toks.front().tok.reg;
                            state.top() = CS_SET;
                            if (!nextToken(this->context, toks)) {
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
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
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
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
                            opcode = zhvm::GetOpcode(toks.front().tok.opr.c_str());

                            if (opcode == OP_UNKNOWN) {
                                ErrorMsg(toks.front().loc, "%s: %s: %s", "SYNTAX ERROR", "unknown opcode", toks.front().tok.opr.c_str());
                                state.push(CS_BAD_END);
                                break;
                            }

                            state.top() = CS_OPEN;
                            if (!nextToken(this->context, toks)) {
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
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
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
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
                            regs[CR_SRC0] = zhvm::RZ;
                            state.top() = CS_COMMA_SRC0;
                            break;
                        case TT2_CLOSE:
                            regs[CR_SRC0] = zhvm::RZ;
                            regs[CR_SRC1] = zhvm::RZ;
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
                            regs[CR_SRC0] = toks.front().tok.reg;
                            state.top() = CS_COMMA_SRC0;
                            if (!nextToken(this->context, toks)) {
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
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
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
                                state.push(CS_BAD_END);
                            }
                            break;
                        case TT2_CLOSE:
                            regs[CR_SRC1] = zhvm::RZ;
                            imm = 0;
                            state.top() = CS_CLOSE;
                            break;
                        default:
                            ErrorMsg(toks.front().loc, "%s: %s", "SYNTAX ERROR", ", or ] expected");
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
                        case TT2_NUMBER_BYTE:
                        case TT2_NUMBER_SHORT:
                        case TT2_NUMBER_LONG:
                        case TT2_NUMBER_QUAD:
                            state.top() = CS_NUMBER;
                            break;
                        case TT2_AT:
                            state.top() = CS_AT;
                            if (!nextToken(this->context, toks)) {
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
                                state.push(CS_BAD_END);
                            }
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
                            regs[CR_SRC1] = toks.front().tok.reg;
                            state.top() = CS_AFTER_SRC1;
                            if (!nextToken(this->context, toks)) {
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
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
                        case TT2_NUMBER_BYTE:
                        case TT2_NUMBER_SHORT:
                        case TT2_NUMBER_LONG:
                        case TT2_NUMBER_QUAD:
                            state.top() = CS_NUMBER;
                            break;
                        case TT2_AT:
                            state.top() = CS_AT;
                            if (!nextToken(this->context, toks)) {
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
                                state.push(CS_BAD_END);
                            }
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
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
                                state.push(CS_BAD_END);
                            }
                            break;
                        case TT2_SIGN_PLUS:
                            signum = 1;
                            state.top() = CS_NUMBER;
                            if (!nextToken(this->context, toks)) {
                                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
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
                    auto& toksfront = toks.front();
                    if ((toksfront.tok.num > ZHVM_IMMVAL_MAX) || (toksfront.tok.num < ZHVM_IMMVAL_MIN)) {
                        ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "14-BIT NUMBER EXPECTED");
                        state.push(CS_BAD_END);
                        break;
                    }
                    imm = toksfront.tok.num;
                    state.top() = CS_CLOSE;
                    if (!nextToken(this->context, toks)) {
                        ErrorMsg(toksfront.loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
                        state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_AT:
                {
                    auto& toksfront = toks.front();
                    switch (toksfront.tok.type) {
                        case TT2_WORD:
                        {
                            auto lb = this->labels.find(toksfront.tok.opr);
                            if (lb == this->labels.end()) { // Expect later declaration

                                this->fixes.insert(std::make_pair(toksfront.tok.opr, this->code_offset));
                                imm = ZHVM_IMMVAL_MAX;

                            } else { // Already declared
                                if (lb->second > ZHVM_IMMVAL_MAX) {
                                    ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "14-BIT NUMBER EXPECTED");
                                    state.top() = CS_BAD_END;
                                    break;
                                }
                                imm = lb->second;
                            }

                            state.top() = CS_CLOSE;
                            if (!nextToken(this->context, toks)) {
                                ErrorMsg(toksfront.loc, "%s: %s", "FORMAT ERROR", "unexpected eof");
                                state.push(CS_BAD_END);
                            }
                            break;
                        }
                        default:
                            ErrorMsg(toks.front().loc, "%s: %s", "SYNTAX ERROR", "WORD expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_CLOSE:
                {
                    auto& toksfront = toks.front();
                    switch (toksfront.tok.type) {
                        case TT2_CLOSE:
                            state.top() = CS_FINISH;
                            nextToken(this->context, toks);
                            break;
                        default:
                            ErrorMsg(toksfront.loc, "%s: %s", "SYNTAX ERROR", "] expected");
                            state.push(CS_BAD_END);
                    }
                    break;
                }
                case CS_FINISH:
                {
                    uint32_t cmd = zhvm::PackCommand(opcode, regs, imm * signum);
                    mem->SetCode(this->code_offset, cmd);
                    this->code_offset += sizeof (uint32_t);

                    LogMsg("0x%04x: 0x%08x", this->code_offset - (uint32_t)sizeof (uint32_t), cmd);

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
                    LogMsg("%s", "ASSEMBLER STATE STACK TRACE");
                    while (!state.empty()) {
                        LogMsg("  %s", statetext[state.top()]);
                        state.pop();
                    }
                    LogMsg("%s", "============================");
                    return TT2_ERROR;
                default:
                    ErrorMsg(toks.front().loc, "%s: %s", "PARSER ERROR", "unknown state");
                    state.push(CS_BAD_END);
            }

        }

        for (auto i = this->fixes.begin(), e = this->fixes.end(); i != e; ++i) {

            auto& name = i->first;
            auto& offs = i->second;

            auto label = this->labels.find(name);
            if (label == this->labels.end()) {
                ErrorMsg(0, "%s: %s %s", "FIXES ERROR", "unknown label", name.c_str());
                state.push(CS_BAD_END);
                return TT2_ERROR;
            }

            zhvm::UnpackCommand(mem->GetCode(offs), &opcode, regs, &imm);

            imm = label->second;

            if ((imm > ZHVM_IMMVAL_MAX) || (imm < ZHVM_IMMVAL_MIN)) {
                ErrorMsg(toks.front().loc, "%s: %s", "FORMAT ERROR", "14-BIT NUMBER EXPECTED");
                state.push(CS_BAD_END);
                return TT2_ERROR;
            }

            uint32_t cmd = zhvm::PackCommand(opcode, regs, imm);

            mem->SetCode(offs, cmd);

        }

        if (state.empty()) {
            return TT2_EOF;
        }

        LogMsg("%s", "ASSEMBLER STATE STACK TRACE");
        while (!state.empty()) {
            LogMsg("  %s", statetext[state.top()]);
            state.pop();
        }
        LogMsg("%s", "============================");
        return TT2_ERROR;
    }


}