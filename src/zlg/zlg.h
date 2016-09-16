/** 
 * @file zlg.h
 * @author timur
 * @date 14.09.2016
 */

#ifndef __ZLG_HEADER__
#define __ZLG_HEADER__

#define YY_NO_UNPUT
#define YY_NO_WRAP

using namespace std;

#include <iostream>
#include <cstdio>
#include <string>

namespace zlg {
    
    struct token {
        std::string text;
        int64_t value;
    };
    
}

#endif /* __ZLG_HEADER__ */

