/**
 * @file cmpl.class.cpp
 * @author timur
 * 
 * Complete ZHVM assembler implementation.
 * 
 */

#include <fstream>
#include <iostream>
#include <iomanip>

#include <cstdarg>
#include <climits>

#include <cmpl.class.h>

namespace zhvm {

    enum cmpl_state {
        CS_START = 0,
        CS_FINISH = 1,
        CS_BAD_END = 2,

        CS_CMD_DEST = 100,
        CS_CMD_OPCODE = 101,
        CS_CMD_SRC0 = 102,
        CS_CMD_COMMA_SRC0 = 103,
        CS_CMD_SRC1 = 104,
        CS_CMD_COMMA_SRC1 = 105,
        CS_CMD_NUMBER = 106,
        CS_CMD_END = 107

    };

    void cmpl::ErrorMsg(const YYLTYPE& loc, const char* format, ...) {

        char buffer[256] = {0};

        va_list va;
        va_start(va, format);
        vsnprintf(buffer, 256, format, va);
        va_end(va);

        fprintf(stderr, "INPUT(%d): %s\n", loc.line, buffer);
    }

    void cmpl::NextToken() {
        this->tok[0] = this->tok[1];
        this->loc[0] = this->loc[1];
        yylex(this->tok + 1, this->loc + 1, this->context);
    }

    const YYSTYPE& cmpl::Token() {
        return this->tok[0];
    }

    const YYSTYPE& cmpl::Lookup() {
        return this->tok[1];
    }

    cmpl::cmpl(FILE* stream, memory * mem) : mem(mem), context(0), tok(), loc(), state() {
        if ((mem == 0) || (stream == 0)) {
            loc[0].line = 0;
            ErrorMsg(loc[0], "%s", "INVALID POINTER TO MEMORY");
            throw std::runtime_error("INVALID POINTER TO MEMORY");
        }

        yylex_init(&this->context);
        yyset_in(stream, this->context);

        // Bootstrapping
        this->NextToken();
        this->NextToken();

        this->state.push(CS_START);

    }

    int cmpl::operator()() {
        return this->Compile();
    }

    cmpl::~cmpl() {
        yylex_destroy(this->context);
    }

    void cmpl::Macros() {
        this->state.push(EC_FATAL);
    }

    void cmpl::Command() {
        this->state.push(CS_CMD_DEST);

        while (this->Token().tok != TOK_EOF) {

            if (this->state.empty()) {
                return EC_NO_STATE;
            }

            switch (this->state.top()) {

                case CS_CMD_DEST:
                { // At start we can gen dest reg or opcode
                    switch (this->Token().tok) {
                        case TOK_SREG: // Token is standart register
                        case TOK_EREG: // Token is extended register
                            regs[0] = this->Token().reg.val;
                            am = AM_OPCODE;
                            NEXT_TOKEN;
                            break;
                        case TOK_EOL: // If we reach eol just skip
                            NEXT_TOKEN;
                            break;
                        case TOK_ID: // If we got token, it can be opcode, so change mode and set dest to RZ
                            regs[0] = zhvm::RZ;
                            am = AM_OPCODE;
                            break;
                    }
                    break;
                }
                case CS_CMD_OPCODE:
                {
                    switch (this->Token().tok) {
                        case TOK_ID: // We must have valid opcode
                            opcode = zhvm::GetOpcode(this->Token().id.val);
                            if (opcode == zhvm::OP_UNKNOWN) { // If get unknown opcode throw error
                                Error("UNKNOWN OPCODE", cloc[0]);
                                goto bad_end;
                            }
                            NEXT_TOKEN;
                            am = AM_SRC0;
                            break;
                        default: // Only opcode expected
                            Error("OPCODE EXPECTED", cloc[0]);
                            goto bad_end;
                    }
                    break;
                }
                case CS_CMD_SRC0:
                {
                    switch (this->Token().tok) {
                        case TOK_SREG: // Token is standart register
                        case TOK_EREG: // Token is extended register
                            regs[1] = this->Token().reg.val;
                            am = AM_COMMA_SRC0;
                            NEXT_TOKEN;
                            break;
                        case TOK_COMMA:
                            regs[1] = zhvm::RZ;
                            am = AM_COMMA_SRC0;
                            break;
                        case TOK_EOL: // If we reach eol we have only opcode
                            regs[1] = zhvm::RZ;
                            am = AM_END;
                            break;
                    }
                    break;
                }
                case CS_CMD_COMMA_SRC0:
                {
                    switch (this->Token().tok) {
                        case TOK_COMMA: // comma as expected
                            am = AM_SRC1;
                            NEXT_TOKEN;
                            break;
                        case TOK_EOL: // end of line
                            am = AM_END;
                            break;
                        default: // Only comma expected
                            Error("COMMA EXPECTED", cloc[0]);
                            goto bad_end;
                    }
                    break;
                }
                case CS_CMD_SRC1:
                {
                    switch (this->Token().tok) {
                        case TOK_SREG: // Token is standart register
                            regs[2] = this->Token().reg.val;
                            am = AM_COMMA_SRC1;
                            NEXT_TOKEN;
                            break;
                        case TOK_EREG: // Extended registers forbidden
                            Error("ONLY $Z, $A,$B,$C REGISTERS EXPECTED", cloc[0]);
                            goto bad_end;
                        case TOK_COMMA:
                            regs[2] = zhvm::RZ;
                            am = AM_COMMA_SRC1;
                            break;
                        case TOK_EOL: // If we reach eol we have only opcode
                            regs[2] = zhvm::RZ;
                            am = AM_END;
                            break;
                    }
                    break;
                }
                case CS_CMD_COMMA_SRC1:
                {
                    switch (this->Token().tok) {
                        case TOK_COMMA: // comma as expected
                            am = AM_NUMBER;
                            NEXT_TOKEN;
                            break;
                        case TOK_EOL: // end of line
                            am = AM_END;
                            break;
                        default: // Only comma expected
                            Error("COMMA EXPECTED", cloc[0]);
                            goto bad_end;
                    }
                    break;
                }
                case CS_CMD_NUMBER:
                {
                    switch (this->Token().tok) {
                        case TOK_NUMBER: // number as expected
                            if ((this->Token().num.val > SHRT_MAX) || (this->Token().num.val < SHRT_MIN)) {
                                Error("16-BIT NUMBER EXPECTED", cloc[0]);
                                goto bad_end;
                            }
                            imm = this->Token().num.val & 0xFFFF;
                            NEXT_TOKEN;
                            am = AM_END;
                            break;
                        case TOK_EOL: // end of line
                            imm = 0;
                            am = AM_END;
                            break;
                    }
                }
                case CS_CMD_END:
                {
                    uint32_t cmd = zhvm::PackCommand(opcode, regs, imm);
                    mem->SetLong(offset, (uint32_t) cmd);
                    offset += sizeof (uint32_t);
                    NEXT_TOKEN;
                    am = AM_AT_START;
                    std::cout << std::hex << "0x" << std::setw(8) << std::setfill('0') << cmd << std::endl;

                    regs[0] = zhvm::RZ;
                    regs[1] = zhvm::RZ;
                    regs[2] = zhvm::RZ;
                    opcode = zhvm::OP_HLT;
                    imm = 0;
                    break;
                }
            }
        }

    }

    int cmpl::Compile() {

        while (this->Token().tok != TOK_EOF) {

            if (this->state.empty()) {
                return EC_NO_STATE;
            }

            switch (this->state.top()) {
                case CS_START:
                {
                    switch (this->Token().tok) {
                        case TOK_MACRO:
                            this->Macros();
                            break;
                        case TOK_EOL:
                            this->NextToken();
                            break;
                        default:
                            this->Command();
                    }
                    break;
                }
                case CS_FINISH:
                {
                    return EC_OK;
                }
                case CS_BAD_END:
                {
                    this->ErrorMsg(this->loc[0], "%s", "FATAL ERROR");
                    return EC_FATAL;
                }
            }
        }

        if (!this->state.empty()) {
            return EC_END_UNEXPECTED;
        }

        return EC_OK;
    }


}