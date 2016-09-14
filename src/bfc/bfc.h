#pragma once
#ifndef __BFC_HEADER__
#define __BFC_HEADER__

#include <cstdint>

namespace bfc {

    enum token_type {
        BT_EOF = 0,
        BT_MOVL_N, ///< N-times <
        BT_MOVR_N, ///< N-times >
        BT_ADD_N, ///< N-times +
        BT_SUB_N, ///< N-times -
        BT_PUT, ///< .
        BT_GET, ///< ,
        BT_LOOP, ///< [
        BT_END, ///< ]
        BT_ZERO, ///< Special case [+] [-]
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
