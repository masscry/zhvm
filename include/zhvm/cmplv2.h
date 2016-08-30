/**
 * @file cmplv2.h
 * @author marko
 *
 * Flex binding
 *
 */

#pragma once
#ifndef __COMPILER_VER_2__
#define __COMPILER_VER_2__

#include <cstdint>
#include <string>

namespace zhvm {

    /**
     * ZHVM assembler token names version 2
     */
    enum token_type_v2 {
        TT2_ERROR = -1,
        TT2_EOF = 0,
        TT2_REG,
        TT2_SET,
        TT2_WORD,
        TT2_OPEN,
        TT2_CLOSE,
        TT2_COMMA,
        TT2_SIGN_MINUS,
        TT2_SIGN_PLUS,
        TT2_NUMBER_BYTE,
        TT2_NUMBER_SHORT,
        TT2_NUMBER_LONG,
        TT2_NUMBER_QUAD,
        TT2_MACRO,
        TT2_AT
    };
    
    /**
     * Unified token
     */
    struct token_v2 {
        token_type_v2 type;
        int64_t num;
        uint32_t reg;
        std::string opr;
    };

    /**
     * Ask flex to provide only line
     */
    typedef int location;


    /**
     * Token type for flex
     */
#define YYSTYPE zhvm::token_v2

    /**
     * Location type for flex
     */
#define YYLTYPE zhvm::location

    /**
     * Structure for inner usage
     */
    struct yydata {
        YYSTYPE tok;
        YYLTYPE loc;
    };

    /**
     * Print error message
     */
    void ErrorMsg(location loc, const char* format, ...);

    /**
     * Print log message
     */
    void LogMsg(const char* format, ...);

}

/**
 * Print error message
 */
#define ERROR_MSG(format, ...) zhvm::ErrorMsg(*yylloc, format, ##__VA_ARGS__)

#endif // __COMPILER_VER_2__
