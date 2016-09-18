/** 
 * @file zlg.cpp
 * @author timur
 * @date 14.09.2016
 */

#include <cstdlib>
#include <zlg.h>
#include <zlgy.gen.hpp>
#include <zlg.gen.h>

void produce(const zlg::node& item) {
    item.print(stdout);
}

int main(int argc, char** argv) {
    yyscan_t scan;

    printf("# %s\n", "ZLG PROGRAM");

    zlg::ast root;

    yylex_init(&scan);
    yyparse(scan, root);
    yylex_destroy(scan);

    for (std::list<std::shared_ptr<zlg::node> >::const_iterator i = root.Items().begin(), e = root.Items().end(); i != e; ++i) {
        produce(**i);
    }

    printf("# %s\n", "END ZLG PROGRAM");

    return 0;
}

