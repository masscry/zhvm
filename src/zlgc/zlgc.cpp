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

    root.Prepare();
    root.Produce(std::cout);

    printf("# %s\n", "END ZLG PROGRAM");

    return 0;
}

