/**
 * @file cmpl.h
 * @author timur
 *
 * Compiler class implementation
 * 
 */

#ifndef __CMPL_CLASS_HEADER__
#define __CMPL_CLASS_HEADER__

#include <stack>

#include <zhvm.h>
#include <zhlex.h>
#include <zhlex.gen.h>

namespace zhvm {

    /**
     * Main compiler class
     */
    class cmpl {
        typedef std::stack<int> intStack;

        memory* mem;
        yyscan_t context;
        YYSTYPE tok[2];
        YYLTYPE loc[2];
        intStack state;

        /**
         * Print error message to stderr.
         * 
         * @param loc error location
         * @param format error message format
         * @param ... arguments
         */
        void ErrorMsg(const YYLTYPE& loc, const char* format, ...);

        /**
         * Get Next token.
         */
        void NextToken();

        /**
         * Current token.
         * 
         * @return token
         */
        const YYSTYPE& Token();

        /**
         * Next token.
         * 
         * @return token
         */
        const YYSTYPE& Lookup();

        /**
         * Compile stream.
         * 
         * @return error code
         * @see error_codes
         */
        int Compile();

        /**
         * Compile command.
         */
        void Command();

        /**
         * Compile macros
         */
        void Macros();

    public:

        /**
         * Create new compiler
         * @param stream stream to compile
         * @param mem memory to fill with compiled program
         * 
         */
        cmpl(FILE* stream, memory * mem);

        /**
         * Compiler stream
         * @return  erro code
         * @see error_codes
         */
        int operator()();

        /**
         * Cleanup
         */
        ~cmpl();

    };

}

#endif // __CMPL_CLASS_HEADER__
