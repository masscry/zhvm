/** 
 * @file zlg.h
 * @author timur
 * @date 14.09.2016
 */

#ifndef __ZLG_HEADER__
#define __ZLG_HEADER__

#define YY_NO_UNPUT
#define YY_NO_WRAP
#define YYERROR_VERBOSE 1
#define YYDEBUG 1

using namespace std;

#include <iostream>
#include <cstdio>
#include <string>
#include <ast.class.h>

namespace zlg {
    
    struct token {
        std::string text;
        int64_t value;
        std::shared_ptr<zlg::node> expr;
    };
    
}

#endif /* __ZLG_HEADER__ */

