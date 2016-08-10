#ifndef __CMPLV2_CLASS_HEADER__
#define __CMPLV2_CLASS_HEADER__

#include <zhvm.h>
#include "cmplv2.h"
#include "cmplv2.gen.h"

namespace zhvm {

    class cmplv2 {
        yyscan_t context;
        memory* mem;

        cmplv2(const cmplv2& copy);

        int invoke();
        
    public:

        int operator()();

        cmplv2(FILE* input, memory* mem);
        ~cmplv2();

    };

}


#endif // __CMPLV2_CLASS_HEADER__