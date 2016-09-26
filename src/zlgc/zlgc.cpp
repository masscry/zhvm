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
    zlg::context cont;

    root.Scan();
    root.Generate(std::cout, &cont);

    printf("# %s\n", "END ZLG PROGRAM");

    return 0;
}

