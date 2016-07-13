#include <iostream>
#include <string>
#include <zhvm.h>
#include <cstring>

enum replcmd {
    RC_NONE = 0,
    RC_EXIT = 1
};

int GetCMD(const std::string &str) {
    if (str[0] == '~'){
        if (str.compare("~exit") == 0) {
            return RC_EXIT;
        }
    }
    return RC_NONE;
}

int main() {

    std::cout << sizeof(zhvm::cmd) << std::endl;

    zhvm::memory mem;

    bool loop = true;
    while (loop) {
        if (std::cin.good()) {
            std::cout << "-> ";

            std::string input;
            std::getline(std::cin, input);

            int cmd = GetCMD(input);
            if (cmd == RC_EXIT){
                loop = false;
                continue;
            }

            zhvm::cmd test = {0};

            const char *result = Assemble(input.c_str(), &test);

            if (result == 0) {
                std::cerr << "Bad instruction: " << input << std::endl;
                continue;
            }

            if (result != 0) {
                if (strlen(result)!=0){
                    std::cout << result << std::endl;
                }
                std::cout << std::hex << "0x" << zhvm::AsBytes(test) << std::endl;
            }

            switch (zhvm::Invoke(&mem, test)){
                case zhvm::IR_HALT:
                    loop = false;
                    std::cout << "HALT VM" << std::endl;
                    break;
                case zhvm::IR_OP_UNKNWN:
                    loop = false;
                    std::cerr << "UNKNOWN VM OPERAND" << std::endl;
                    break;
                case zhvm::IR_RUN:
                    break;
            }

            mem.Print(std::cout);

        }
    }

    return 0;
}
