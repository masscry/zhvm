/**
 * @author marko
 * @date 07.09.2016
 * 
 * ZHVM image executor
 * 
 */

#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <zhvm.h>
#include <zhtime.h>

using namespace zhvm;

const char* inputname = 0;
bool burst = false;
bool verbose = true;

enum arguments {
    PA_START,
    PA_INPUT,
    PA_BURST,
    PA_SILENT
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
                        case 'b':
                            mode = PA_BURST;
                            break;
                        case 's':
                            mode = PA_SILENT;
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
            case PA_BURST:
            {
                burst = true;
                mode = PA_START;
                ++i;
                break;
            }
            case PA_SILENT:
            {
                verbose = false;
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
    }
    fprintf(stderr, "%s: %s\n", "ERROR", "can't reach here");
    return -1;
}

int vm_put(zhvm::memory* mem) {
    std::cout << std::hex << "0x" << mem->Get(zhvm::RA) << std::endl;
    return zhvm::IR_RUN;
}

int vm_get(zhvm::memory* mem) {
    int val = 0;
    std::cin >> val;
    mem->Set(zhvm::RA, val);
    return zhvm::IR_RUN;
}

int main(int argc, char* argv[]) {

    if (parse_args(argc, argv) != 0) {
        fprintf(stdout, "%s: %s %s\n", "Usage", argv[0], "[-i INPUT] [-b] [-s]");
        return -1;
    }

    std::istream* input = 0;
    std::ifstream inputf;

    if (inputname == 0) {
        input = &std::cin;
        if (verbose) {
            fprintf(stderr, "%s: %s\n", "USE INPUT", "stdin");
        }
    } else {
        inputf.open(inputname, std::ios_base::in);
        if (!inputf) {
            if (verbose) {
                fprintf(stderr, "%s: %s %s (%s)\n", "ERROR", "Failed to open file", inputname, strerror(errno));
            }
            return -1;
        }
        if (verbose) {
            fprintf(stderr, "%s: %s\n", "USE INPUT", inputname);
        }
        input = &inputf;
    }

    memory mem;
    mem.Load(*input);
    mem.SetFuncs(zhvm::CN_PUT, vm_put);
    mem.SetFuncs(zhvm::CN_GET, vm_get);

    
    TD_TIME start;
    TD_TIME stop;
    int result = IR_HALT;

    if (burst) {
        zhtime(&start);
        result = ExecutePrefetch(&mem);
        zhtime(&stop);
    } else {
        zhtime(&start);
        result = Execute(&mem);
        zhtime(&stop);
    }

    if (verbose) {
        std::cout << "EXECUTION TIME: " << zhvm::time_diff(start, stop) << " SEC" << std::endl;
        switch (result) {
            case zhvm::IR_HALT:
                std::cout << "HALT VM" << std::endl;
                break;
            case zhvm::IR_OP_UNKNWN:
                std::cerr << "UNKNOWN VM OPERAND" << std::endl;
                break;
            default:
                std::cerr << "UNHANDLED VM STATE" << std::endl;
        }
        mem.Print(std::cout);
    } else {
        std::cout << zhvm::time_diff(start, stop) << std::endl;        
    }

    return 0;
}