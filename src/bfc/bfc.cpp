#include <bfc.h>
#include <bfc.gen.h>
#include <cstdio>
#include <cstdint>
#include <stack>
#include <stdexcept>
#include <cstdlib>
#include <cstring>

int main(int argc, char* argv[]) {
    using namespace bfc;

    yyscan_t scanner = 0;
    yylex_init(&scanner);
    int result = 0;

    try {

        token tok;
        location loc;
        int result = 0;

        bool loop = true;

        uint32_t counter = 0;
        std::stack<uint32_t> counter_stack;

        printf("# BRAINFUCK ZHVM PROGRAM\n");
        while (loop) {
            result = yylex(&tok, &loc, scanner);
            switch (result) {
                case BT_EOF:
                    loop = false;
                    break;
                case BT_MOVL_N:
                    printf("$a = add[$a, %d]\n", tok.count);
                    counter += 1;
                    break;
                case BT_MOVR_N:
                    printf("$a = sub[$a, %d]\n", tok.count);
                    counter += 1;
                    break;
                case BT_ADD_N:
                    printf("$b = ldb[$a]\n");
                    printf("$b = add[$b, %d]\n", tok.count);
                    printf("$a = svb[$b]\n");
                    counter += 3;
                    break;
                case BT_SUB_N:
                    printf("$b = ldb[$a]\n");
                    printf("$b = sub[$b, %d]\n", tok.count);
                    printf("$a = svb[$b]\n");
                    counter += 3;
                    break;
                case BT_PUT:
                    printf("$c = add[$a]\n");
                    printf("$a = ldb[$a]\n");
                    printf("cll[,2]\n");
                    printf("$a = add[$c]\n");
                    counter += 4;
                    break;
                case BT_GET:
                    printf("$c = add[$a]\n");
                    printf("cll[,3]\n");
                    printf("$c = svb[$a]\n");
                    printf("$a = add[$c]\n");
                    counter += 4;
                    break;
                case BT_LOOP:
                    printf("$b = ldb[$a]\n");
                    printf("$p = cmz[$b, @end%x]\n", counter);
                    printf("!label%x\n", counter);
                    counter_stack.push(counter);
                    counter += 2;
                    break;
                case BT_END:

                    if (counter_stack.empty()) {
                        throw std::runtime_error("Unexpected closing square brace");
                    }

                    printf("$b = ldb[$a]\n");
                    printf("$p = cmn[$b, @label%x]\n", counter_stack.top());
                    printf("!end%x\n", counter_stack.top());
                    counter_stack.pop();
                    counter += 2;
                    break;
            }
        };

        if (!counter_stack.empty()) {
            throw std::runtime_error("Closing brace expected");
        }

        printf("hlt[]\n");
        printf("# PROGRAM SIZE: 0x%x\n", (counter + 1)*4);

    } catch (const std::exception& exp) {
        fprintf(stderr, "bfc error: %s", exp.what());
        result = -1;
    }

    yylex_destroy(scanner);
    return result;
}
