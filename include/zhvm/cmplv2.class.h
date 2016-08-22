#ifndef __CMPLV2_CLASS_HEADER__
#define __CMPLV2_CLASS_HEADER__

#include <zhvm.h>
#include "cmplv2.h"
#include <queue>

typedef void* yyscan_t;

namespace zhvm {

    class cmplv2 {
        uint32_t offset;
        yyscan_t context;
        memory* mem;

        cmplv2(const cmplv2& copy);

        int command();
        int macro(std::queue<yydata>* toks);

    public:

        int operator()();

        cmplv2(FILE* input, memory* mem);
        ~cmplv2();
        
        uint32_t Offset() const;

    };

}


#endif // __CMPLV2_CLASS_HEADER__