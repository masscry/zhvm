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

using namespace zhvm;

enum error_codes{
    EC_INVALID_POINTER = -2,
    EC_BAD_INSTRUCTION = -1,
    EC_OK = 0
};

enum assemble_mode {
  AM_DEST,
  AM_OPCODE,
  AM_SRC0,
  AM_COMMA_SRC0,
  AM_SRC1,
  AM_COMMA_SRC1,
  AM_NUMBER
};

int AssembleProgram(memory* mem){

  if (mem == 0){
    return EC_INVALID_POINTER;
  }

  yyscan_t scan;
  yylex_init(&scan);

  YYSTYPE ctok; // current token
  YYSTYPE ntok; // next token

  YYLTYPE cloc; // current location
  YYLTYPE nloc; // next location

  int cmode; // current mode
  int nmode; // next mode

  uint32_t regs[3];
  uint32_t opcode;
  int16_t  imm;

  // bootstrapping
  cmode = yylex(&ctok, &cloc, scan);
  nmode = yylex(&ntok, &nloc, scan);

  int am = AM_DEST;
  while (cmode != zhvm::TOK_EOF){

    switch (am) {
    case AM_DEST:
      if ((cmode == zhvm::TOK_SREG)||(cmode == zhvm::TOK_EREG)){
        regs[0] = ctok.reg.val;
        am = AM_OPCODE;
      } else {
        regs[0] = zhvm::RZ;
        am = AM_OPCODE;
      }
      break;
    case AM_OPCODE:
      if (cmode == zhvm::TOK_ID){
        opcode = zhvm::GetOpcode(ctok.id.val);
        if (opcode == zhvm::OP_UNKNOWN) {
          std::cerr << "BAD OPCODE: " << cloc.line<< ": " << cloc.col << std::endl;
          goto bad_end;
        }
        am = AM_SRC0;
      } else {
        std::cerr << "OPCODE EXPECTED: " << cloc.line << ": " << cloc.col << std::endl;
        goto bad_end;
      }
      break;
    case AM_SRC0:
      if ((cmode == zhvm::TOK_SREG)||(cmode == zhvm::TOK_EREG)){
        regs[1] = ctok.reg.val;
        am = AM_COMMA;
      } else {
        regs[1] = zhvm::RZ;
        am = AM_COMMA;
      }
      break;
    case AM_COMMA_SRC0:

      

      break;
    case AM_SRC1:
      break;
    case AM_COMMA_SRC1:
      break;
    case AM_NUMBER:
      break;
    }

    cmode = nmode;
    ctok = ntok;
    cloc = nloc;
    nmode = yylex(&ntok, &nloc, scan);
  }

  yylex_destroy(scan);

  return EC_OK;

bad_end:

  yylex_destroy(scan);
  return EC_BAD_INSTRUCTION;

}



int main(int argc, char* argv[]){

  memory mem;

  if (AssembleProgram(&mem) != EC_OK) {
    return -1;
  }

  mem.Dump();

  return 0;
}
