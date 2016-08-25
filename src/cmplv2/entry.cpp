#include <cstdio>
#include <cstring>
#include <zhvm.h>

using namespace zhvm;

int main(int argc, char* argv[]) {

    FILE* input = 0;

    switch (argc) {
        case 1:
            fprintf(stdout, "%s\n", "USE STDIN");
            input = stdin;
            break;
        case 2:
            fprintf(stdout, "%s %s\n", "USE", argv[1]);
            input = fopen(argv[1], "r");
            if (input == 0) {
                fprintf(stderr, "%s: %s: %s\n", "FILE OPEN FAILED", "ERRNO", strerror(errno));
                return -1;
            }
            break;
        default:
            fprintf(stdout, "%s: %s: %s\n", "Usage: ", argv[0], "FILENAME");
            return -1;
    }

    memory mem;
    cmplv2 cmpl(input, &mem);

    if (cmpl() != TT2_EOF) {
        return -1;
    }

    fprintf(stdout, "%s: %#x\n", "PROGRAM SIZE", cmpl.Offset());

    mem.Dump();

    if ((input != stdin) && (input != 0)) {
        fclose(input);
        input = 0;
    }

    return 0;
}