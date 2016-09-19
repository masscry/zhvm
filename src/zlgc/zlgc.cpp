/** 
 * @file zlg.cpp
 * @author timur
 * @date 14.09.2016
 */

#include <cstdlib>
#include <zlg.h>

int main(int argc, char** argv) {

    printf("# %s\n", "ZLG PROGRAM");

    zlg::ast root;

    root.Scan();

    for (std::list<std::shared_ptr<zlg::node> >::const_iterator i = root.Items().begin(), e = root.Items().end(); i != e; ++i) {
        (*i)->Produce(std::cout);
    }

    printf("# %s\n", "END ZLG PROGRAM");

    return 0;
}

