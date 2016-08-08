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

        fprintf(stderr, "INPUT(%d): %s\n", loc, buffer);
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
            loc[0] = 0;
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
        this->state.push(EC_FATAL);        
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