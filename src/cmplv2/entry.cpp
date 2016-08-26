#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <zhvm.h>

using namespace zhvm;

const char* inputname = 0;
const char* outputname = 0;
size_t memsize = 1024;

enum arguments {
    PA_START,
    PA_INPUT,
    PA_OUTPUT,
    PA_SIZE
};

int parse_args(int argc, char* argv[]) {

    int mode = PA_START;
    int i = 1;
    while (i < argc) {
        switch (mode) {
            case PA_START:
            {
                if (strlen(argv[i]) != 2) {
                    fprintf(stderr, "%s: %s %s\n", "ERROR", "two character argument expected", argv[i]);
                    return -1;
                }

                if (argv[i][0] == '-') {
                    switch (argv[i][1]) {
                        case 'i':
                            mode = PA_INPUT;
                            ++i;
                            break;
                        case 'o':
                            mode = PA_OUTPUT;
                            ++i;
                            break;
                        case 's':
                            mode = PA_SIZE;
                            ++i;
                            break;
                        case 'h':
                            return -1;
                        default:
                            fprintf(stderr, "%s: %s %s\n", "ERROR", "unknown argument", argv[i]);
                            return -1;
                    }
                } else {
                    fprintf(stderr, "%s: %s %s\n", "ERROR", "argument expected", argv[i]);
                    return -1;
                }
                break;
            }
            case PA_INPUT:
            {
                inputname = argv[i];
                mode = PA_START;
                ++i;
                break;
            }
            case PA_OUTPUT:
            {
                outputname = argv[i];
                mode = PA_START;
                ++i;
                break;
            }
            case PA_SIZE:
            {
                memsize = strtol(argv[i], 0, 0);
                mode = PA_START;
                ++i;
                break;
            }
            default:
                fprintf(stderr, "%s: %s %s\n", "ERROR", "invalid state", argv[i]);
                return -1;
        }
    }

    switch (mode) {
        case PA_START:
            return 0;
        case PA_INPUT:
            fprintf(stderr, "%s: %s\n", "ERROR", "input filename expected");
            return -1;
        case PA_OUTPUT:
            fprintf(stderr, "%s: %s\n", "ERROR", "output filename expected");
            return -1;

    }
    fprintf(stderr, "%s: %s\n", "ERROR", "can't reach here");
    return -1;
}

int main(int argc, char* argv[]) {

    if (parse_args(argc, argv) != 0) {
        fprintf(stdout, "%s: %s %s\n", "Usage", argv[0], "[-i INPUT] [-o OUTPUT] [-s SIZE]");
        return -1;
    }

    FILE* input = 0;
    std::ostream* output = 0;
    std::ofstream outpf;

    if (inputname == 0) {
        input = stdin;
        fprintf(stderr, "%s: %s\n", "USE INPUT", "stdin");
    } else {
        input = fopen(inputname, "r");
        if (input == 0) {
            fprintf(stderr, "%s: %s %s (%s)\n", "ERROR", "Failed to open file", inputname, strerror(errno));
            return -1;
        }
        fprintf(stderr, "%s: %s\n", "USE INPUT", inputname);
    }

    if (outputname == 0) {
        output = &std::cout;
        fprintf(stderr, "%s: %s\n", "USE OUTPUT", "stdout");
    } else {
        outpf.open(outputname, std::ios_base::out | std::ios_base::binary);
        if (outpf.fail()) {
            fprintf(stderr, "%s: %s %s\n", "ERROR", "Failed to open file", outputname);
            return -1;
        }
        output = &outpf;
        fprintf(stderr, "%s: %s\n", "USE OUTPUT", outputname);
    }

    memory mem(memsize);
    cmplv2 cmpl(input, &mem);

    if (cmpl() != TT2_EOF) {
        return -1;
    }

    fprintf(stderr, "%s: %#x\n", "PROGRAM SIZE", cmpl.Offset());

    mem.Dump(*output);

    if ((input != stdin) && (input != 0)) {
        fclose(input);
        input = 0;
    }

    if ((output != &std::cout)) {
        outpf.close();
        output = 0;
    }

    return 0;
}