/**
 * @file cmplv2.class.h
 * @author marko
 *
 * ZHVM assembler v2 implementation 
 *
 */

#ifndef __CMPLV2_CLASS_HEADER__
#define __CMPLV2_CLASS_HEADER__

#include "cmplv2.h"
#include "constants.h"
#include <queue>
#include <unordered_map>
#include <string>

/**
 * Original declared in cmplv2.gen.h
 */
typedef void* yyscan_t;

/**
 * Original declared in cmplv2.gen.h
 */
struct yy_buffer_state;

/**
 * Original declared in cmplv2.gen.h
 */
typedef yy_buffer_state* YY_BUFFER_STATE;

namespace zhvm {

    /**
     * Synonym for storing code labels
     */
    typedef std::unordered_map<std::string, uint32_t> labels_t;
    
    /**
     * Synonym for storing code offsets to for yet not declared labels
     */
    typedef std::unordered_multimap<std::string, uint32_t> fixes_t;

    /**
     * Class assembles code for ZHVM v2.
     * 
     * !const
     * !0x100
     * 
     * !label
     * $W = opcode [ $X, $Y + IMM ]
     * $X = opcode [$X, @@const]
     * 
     */
    class cmplv2 {
        labels_t labels; ///< Store defined labels
        fixes_t fixes; ///< Store offset where labels must be defined
        
        uint32_t code_offset; ///< code offset
        uint32_t data_offset; ///< data offset
        uint32_t* cur_offset; ///< current offset
        
        yyscan_t context; ///< Scanner context
        YY_BUFFER_STATE bs; ///< Storage for string data
        memory* mem; ///< Destination VM memory

        cmplv2(const cmplv2& copy); ///< Forbids copy
        cmplv2& operator=(const cmplv2& copy); ///< Forbids copy

        /**
         * Main parsing function.
         * 
         * @return TT2_EOF if no error, TT2_ERROR otherwise.
         */
        int command();

        /**
         * Macro parsing function
         * 
         * @param toks current token queue
         * @return TT2_EOF if no error, TT2_ERROR otherwise
         */
        int macro(std::queue<yydata>* toks);

    public:

        /**
         * 
         * Assemble code.
         * 
         * @return TT2_EOF if no error, TT2_ERROR otherwise
         */
        int operator()();

        /**
         * 
         * Prepare assembling c-string
         * 
         * @param input simple c-string
         * @param mem destination VM memory
         */
        cmplv2(const char* input, memory* mem);

        /**
         * 
         * Prepare assembling file stream
         * 
         * @param input file stream
         * @param mem destination VM memory
         */
        cmplv2(FILE* input, memory* mem);

        /**
         * Destructor
         */
        ~cmplv2();

        /**
         * Returns current code offset in VM memory
         * 
         * @return code offset
         */
        uint32_t CodeOffset() const;
        
        
        /**
         * Returns current data offset in VM memory
         * 
         * @return code offset
         */
        uint32_t DataOffset() const;

    };

}


#endif // __CMPLV2_CLASS_HEADER__