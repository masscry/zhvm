/** 
 * @file zlg.cpp
 * @author timur
 * @date 14.09.2016
 */

#include <cstdlib>
#include <zlg.h>

int main(int argc, char** argv) {

    zlg::ast root;
    zlg::context cont;

    root.Scan();

    printf("# %s\n", "ZLG PROGRAM");

    root.Generate(std::cout, &cont, 0);

    printf("# %s\n\n", "END ZLG PROGRAM");

    return 0;
    
}

