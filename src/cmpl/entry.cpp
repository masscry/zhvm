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








int main(int argc, char* argv[]){


    return 0;
}
