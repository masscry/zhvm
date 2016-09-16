/** 
 * @file zlg.cpp
 * @author timur
 * @date 14.09.2016
 */

#include <cstdlib>
#include <zlg.h>
#include <zlgy.gen.hpp>
#include <zlg.gen.h>

int main(int argc, char** argv) {
    yyscan_t scan;
    yylex_init(&scan);

    yyparse(scan);

    yylex_destroy(scan);
    return 0;
}

