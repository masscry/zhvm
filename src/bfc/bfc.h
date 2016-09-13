#pragma once
#ifndef __BFC_HEADER__
#define __BFC_HEADER__

#include <cstdint>

namespace bfc {

    enum token_type {
        BT_EOF = 0,
        BT_MOVL_N,
        BT_MOVR_N,
        BT_ADD_N,
        BT_SUB_N,
        BT_PUT,
        BT_GET,
        BT_LOOP,
        BT_END,
        BT_UNDEF
    };

    struct token {
        token_type type;
        int count;
    };

    typedef int32_t location;

}

#define YYSTYPE bfc::token
#define YYLTYPE bfc::location

#endif // __BFC_HEADER__
