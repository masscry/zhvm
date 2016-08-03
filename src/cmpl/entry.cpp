/**
 * @file entry.cpp
 * @author marko
 * @date 22 July 2016
 *
 * ZHVM macro assembler
 * ====================
 *
 * Basic operand syntax
 * --------------------
 *
 * (DEST)* OPCODE (SRC0)*(,SRC1)*(,IMM)*
 *
 * Elements with * can be ommited. Registers would be substituted by $Z and IMM by 0.
 *
 * Code comments
 * -------------
 *
 * Comments starts with % and end at the end of line
 *
 * Example:
 *
 * @code
 *
 * $a add ,,100 % Comment example
 *
 * @endcode
 *
 *
 * Macro definition
 * ----------------
 *
 * ~macro    - start new macro sequence (short form ~m).
 *
 * ~endmacro - end macro sequence (short form ~em).
 *
 * Example:
 *
 * @code
 *
 * ~macro mname #0,#1,...#N  % Define macro mname with N parameters
 *   cmd1
 *   cmd2
 *   ...
 *   cmdN
 * ~endmacro
 *
 * @endcode
 *
 * Include source
 * --------------
 *
 * Sources includes with ~include command (short form ~i). Filename in braces
 *
 * Example:
 *
 * @code
 *
 * ~include [macro.zhs]
 *
 * @endcode
 *
 *
 * Virtual Machine memory manipulations
 * ------------------------------------
 *
 * Several commands places bytes of defined length in VM memory.
 *
 * ~byte NUMBER  - put one byte
 * ~short NUMBER - put two bytes
 * ~long NUMBER  - put four bytes
 * ~quad NUMBER  - put eight bytes
 *
 * or in short form
 *
 * ~b NUMBER  - put one byte
 * ~s NUMBER - put two bytes
 * ~l NUMBER  - put four bytes
 * ~q NUMBER  - put eight bytes
 *
 * Labels
 * ------
 *
 * Labels used to mark code offsets, to be used in later code.
 *
 * Label are defined by ~label command (short form ~@).
 *
 * ~label N - marks current VM memory cursor position with name N
 *
 * Assembler substitues any @@N with offset value.
 *
 * Example:
 *
 * @code
 *
 * ~@ value        % Define new label @@value
 * ~s 100          % Place two bytes with value 100 at current offset
 *
 * $a add ,,@value % Set RA to @@value offset
 *
 * $b lds $a,,     % Load to RB value from memory at RA offset
 *
 * @endcode
 *
 * Constants
 * ---------
 *
 * Constants can be defined with some value, to be used later in code.
 *
 * ~const N V - defines constant with name N and value V.
 *
 * Assembler substitues any !N with constant value.
 *
 */

#include <zhvm.h>
#include <fstream>
#include <iostream>
#include <cstring>

#include <zhlex.h>
#include <zhlex.gen.h>
#include <limits.h>
#include <iomanip>

using namespace zhvm;

enum error_codes {
    EC_INVALID_POINTER = -2,
    EC_BAD_INSTRUCTION = -1,
    EC_OK = 0
};

enum assemble_mode {
    AM_AT_START,
    AM_MACRO_PROCESS,
    AM_DEST,
    AM_OPCODE,
    AM_SRC0,
    AM_COMMA_SRC0,
    AM_SRC1,
    AM_COMMA_SRC1,
    AM_NUMBER,
    AM_END
};

int AssembleProgram(FILE* input, memory* mem) {

    if (mem == 0) {
        return EC_INVALID_POINTER;
    }

    yyscan_t scan;
    yylex_init(&scan);
    yyset_in(input, scan);
    

    YYSTYPE ctok; // current token
    YYSTYPE ntok; // next token

    YYLTYPE cloc; // current location
    YYLTYPE nloc; // next location

    int cmode; // current mode
    int nmode; // next mode

    uint32_t regs[3] = {zhvm::RZ, zhvm::RZ, zhvm::RZ};
    uint32_t opcode = zhvm::OP_HLT;
    int16_t imm = 0;

    // bootstrapping
    cmode = yylex(&ctok, &cloc, scan);
    nmode = yylex(&ntok, &nloc, scan);

    int am = AM_AT_START;
    uint32_t offset = 0;

#define NEXT_TOKEN cmode = nmode; ctok = ntok; cloc = nloc; nmode = yylex(&ntok, &nloc, scan)

    while (cmode != zhvm::TOK_EOF) {

        if (cmode == zhvm::TOK_ERROR) {
            std::cerr << "SYNTAX ERROR: " << cloc.line << ": " << cloc.col << std::endl;
            goto bad_end;
        }

        switch (am) {
            case AM_AT_START:
            { // at very start we expects to have macro or simple opcode
                switch (cmode) {
                    case zhvm::TOK_MACRO:
                        am = AM_MACRO_PROCESS;
                        break;
                    default:
                        am = AM_DEST;
                }
                break;
            }
            case AM_MACRO_PROCESS:
            {
                std::cerr << "UNKNOWN MACRO: " << cloc.line << ": " << ctok.id.val << std::endl;
                goto bad_end;
                break;
            }
            case AM_DEST:
            { // At start we can gen dest reg or opcode
                switch (cmode) {
                    case zhvm::TOK_SREG: // Token is standart register
                    case zhvm::TOK_EREG: // Token is extended register
                        regs[0] = ctok.reg.val;
                        am = AM_OPCODE;
                        NEXT_TOKEN;
                        break;
                    case zhvm::TOK_EOL: // If we reach eol just skip
                        NEXT_TOKEN;
                        break;
                    case zhvm::TOK_ID: // If we got token, it can be opcode, so change mode and set dest to RZ
                        regs[0] = zhvm::RZ;
                        am = AM_OPCODE;
                        break;
                }
                break;
            }
            case AM_OPCODE:
            {
                switch (cmode) {
                    case zhvm::TOK_ID: // We must have valid opcode
                        opcode = zhvm::GetOpcode(ctok.id.val);
                        if (opcode == zhvm::OP_UNKNOWN) { // If get unknown opcode throw error
                            std::cerr << "UNKNOWN OPCODE: " << cloc.line << ": " << cloc.col << std::endl;
                            goto bad_end;
                        }
                        NEXT_TOKEN;
                        am = AM_SRC0;
                        break;
                    default: // Only opcode expected
                        std::cerr << "OPCODE EXPECTED: " << cloc.line << ": " << cloc.col << std::endl;
                        goto bad_end;
                }
                break;
            }
            case AM_SRC0:
            {
                switch (cmode) {
                    case zhvm::TOK_SREG: // Token is standart register
                    case zhvm::TOK_EREG: // Token is extended register
                        regs[1] = ctok.reg.val;
                        am = AM_COMMA_SRC0;
                        NEXT_TOKEN;
                        break;
                    case zhvm::TOK_COMMA:
                        regs[1] = zhvm::RZ;
                        am = AM_COMMA_SRC0;
                        break;
                    case zhvm::TOK_EOL: // If we reach eol we have only opcode
                        regs[1] = zhvm::RZ;
                        am = AM_END;
                        break;
                }
                break;
            }
            case AM_COMMA_SRC0:
            {
                switch (cmode) {
                    case zhvm::TOK_COMMA: // comma as expected
                        am = AM_SRC1;
                        NEXT_TOKEN;
                        break;
                    case zhvm::TOK_EOL: // end of line
                        am = AM_END;
                        break;
                    default: // Only comma expected
                        std::cerr << "COMMA EXPECTED: " << cloc.line << ": " << cloc.col << std::endl;
                        goto bad_end;
                }
                break;
            }
            case AM_SRC1:
            {
                switch (cmode) {
                    case zhvm::TOK_SREG: // Token is standart register
                        regs[2] = ctok.reg.val;
                        am = AM_COMMA_SRC1;
                        NEXT_TOKEN;
                        break;
                    case zhvm::TOK_EREG: // Extended registers forbidden
                        std::cerr << "ONLY $A,$B,$C or $Z expected: " << cloc.line << ": " << cloc.col << std::endl;
                        goto bad_end;
                    case zhvm::TOK_COMMA:
                        regs[2] = zhvm::RZ;
                        am = AM_COMMA_SRC1;
                        break;
                    case zhvm::TOK_EOL: // If we reach eol we have only opcode
                        regs[2] = zhvm::RZ;
                        am = AM_END;
                        break;
                }
                break;
            }
            case AM_COMMA_SRC1:
            {
                switch (cmode) {
                    case zhvm::TOK_COMMA: // comma as expected
                        am = AM_NUMBER;
                        NEXT_TOKEN;
                        break;
                    case zhvm::TOK_EOL: // end of line
                        am = AM_END;
                        break;
                    default: // Only comma expected
                        std::cerr << "COMMA EXPECTED: " << cloc.line << ": " << cloc.col << std::endl;
                        goto bad_end;
                }
                break;
            }
            case AM_NUMBER:
            {
                switch (cmode) {
                    case zhvm::TOK_NUMBER: // number as expected
                        if ((ctok.num.val > SHRT_MAX) || (ctok.num.val < SHRT_MIN)) {
                            std::cerr << "16-BIT NUMBER EXPECTED: " << cloc.line << ": " << cloc.col << std::endl;
                            goto bad_end;
                        }
                        imm = ctok.num.val & 0xFFFF;
                        NEXT_TOKEN;
                        am = AM_END;
                        break;
                    case zhvm::TOK_EOL: // end of line
                        imm = 0;
                        am = AM_END;
                        break;
                }
            }
            case AM_END:
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

    std::cout << "PROGRAM SIZE: " << offset << std::endl;
    std::cout << "DONE." << std::endl;

    yylex_destroy(scan);

    return EC_OK;

bad_end:

    yylex_destroy(scan);
    return EC_BAD_INSTRUCTION;

}

int main(int argc, char* argv[]) {

    FILE* input = 0;

    switch (argc) {
        case 1:
            std::cout << "BUILD STDIN" << std::endl;
            input = stdin;
            break;
        case 2:
            std::cout << "BUILD " << argv[1] << std::endl;
            input = fopen(argv[1], "r");
            if (input == 0){
                std::cout << "FILE OPEN FAILED" << std::endl;
                std::cout << "ERRNO: " << strerror(errno) << std::endl;
                return -1;
            }            
            break;
        default:
            std::cout << "Usage: " << argv[0] << " FILENAME" << std::endl;
            std::cout << "   Or: " << argv[0] << std::endl;
            return -1;
    }

    memory mem;

    if (AssembleProgram(input, &mem) != EC_OK) {
        return -1;
    }

    mem.Dump();
    
    if ((input != stdin) && (input != 0)){
        fclose(input);
        input = 0;
    }

    return 0;
}
