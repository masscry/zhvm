#ifndef __CMPLV2_CLASS_HEADER__
#define __CMPLV2_CLASS_HEADER__

#include "cmplv2.h"
#include "constants.h"
#include <queue>
#include <unordered_map>
#include <string>

typedef void* yyscan_t;
struct yy_buffer_state;

typedef yy_buffer_state* YY_BUFFER_STATE;

namespace zhvm {

    typedef std::unordered_map<std::string, uint32_t> labels_t;
    typedef std::unordered_multimap<std::string, uint32_t> fixes_t;

    class cmplv2 {
        labels_t labels;
        fixes_t fixes;
        uint32_t offset;
        yyscan_t context;
        YY_BUFFER_STATE bs;
        memory* mem;

        cmplv2(const cmplv2& copy);
        cmplv2& operator=(const cmplv2& copy);

        int command();
        int macro(std::queue<yydata>* toks);

    public:

        int operator()();

        cmplv2(const char* input, memory* mem);
        cmplv2(FILE* input, memory* mem);
        ~cmplv2();

        uint32_t Offset() const;

    };

}


#endif // __CMPLV2_CLASS_HEADER__