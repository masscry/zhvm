#include <iostream>
#include <string>
#include <zhvm.h>
#include <sman.h>
#include <cstring>

enum replcmd {
    RC_NONE,
    RC_EXIT,
    RC_INTRO,
    RC_LIST,
    RC_DUMP,
    RC_LOAD,
    RC_EXEC,
    RC_TOTAL
};

int GetCMD(const std::string &str) {
    if (str[0] == '~'){
        if (str.compare("~exit") == 0) {
            return RC_EXIT;
        }
        if (str.compare("~intro") == 0) {
            return RC_INTRO;
        }
        if (str.compare("~list") == 0) {
            return RC_LIST;
        }
        if (str.compare("~dump") == 0) {
            return RC_DUMP;
        }
        if (str.compare("~load") == 0) {
            return RC_LOAD;
        }
        if (str.compare("~exec") == 0) {
            return RC_EXEC;
        }        
    }
    return RC_NONE;
}

namespace {

    const char* cmdlist[RC_TOTAL+1] = {
        "  [dest] opt [s0],[s1],[im] - full command syntax. [*] can be ommited.",
        "  ~exit                     - close interpreter.",
        "  ~intro                    - show intro (this text).",
        "  ~list                     - show operand list.",
        "  ~dump                     - dump current vm memory to file \"dump.bin\".",
        "  ~load                     - load file \"dump.bin\" to vm memory.",
        "  ~exec                     - execute program in vm memory from $p offset.",
        0
    };

    const char* oplist[zhvm::OP_TOTAL+1] = {
        "  hlt [0x00] HALT VM",
        
        "  add [0x01] D = S0 + (S1 + IM)",
        "  sub [0x02] D = S0 - (S1 + IM)",
        "  mul [0x03] D = S0 * (S1 + IM)",
        "  div [0x04] D = S0 / (S1 + IM)",
        "  mod [0x05] D = S0 % (S1 + IM)",
        
        "  cmz [0x06] IF (S0 == 0) D = (S1+IM)",
        "  cmn [0x07] IF (S0 != 0) D = (S1+IM)",

        "  ldb [0x08] D = (1 byte)mem[S0+S1+IM]",
        "  lds [0x09] D = (2 bytes)mem[S0+S1+IM]",
        "  ldl [0x0A] D = (4 bytes)mem[S0+S1+IM]",
        "  ldq [0x0B] D = (8 bytes)mem[S0+S1+IM]",

        "  svb [0x0C] mem[D+S1+IM] = (1 byte)S0",
        "  svs [0x0D] mem[D+S1+IM] = (2 bytes)S0",
        "  svl [0x0E] mem[D+S1+IM] = (4 bytes)S0",
        "  svq [0x0F] mem[D+S1+IM] = (8 bytes)S0",
        
        "  and [0x10] D = S0 & (S1 + IM)",
        "   or [0x11] D = S0 | (S1 + IM)",
        "  xor [0x12] D = S0 ^ (S1 + IM)",
        0
    };

}

void PrintWelcomeList(const char* welcome, const char** list){
    std::cout << welcome << std::endl;
    const char** cursor = list;
    while(*cursor != 0){
        std::cout << *cursor << std::endl;
        ++cursor;
    }    
}

void PrintIntro(){
    PrintWelcomeList("Welcome!\nUsage:", cmdlist);
}

void PrintList(){
    PrintWelcomeList("ZHVM operand list:", oplist);
}

enum repl_state {
  RS_BREAK = 0,
  RS_NEXT = 1,
  RS_CMD = 2
};

int replRound(std::istream& istrm, zhvm::memory* mem){

    std::string input;
    std::getline(istrm, input);

    int cmd = GetCMD(input);
    
    switch (cmd){
    case RC_EXIT:
        return RS_BREAK;
    case RC_INTRO:
        PrintIntro();
        return RS_CMD;
    case RC_LIST:
        PrintList();
        return RS_CMD;
    case RC_DUMP:
        mem->Dump();
        return RS_CMD;
    case RC_LOAD:
        mem->Load();
        return RS_CMD;
    case RC_EXEC:
        switch (zhvm::Execute(mem)){
        case zhvm::IR_HALT:
            std::cout << "HALT VM" << std::endl;
            break;
        case zhvm::IR_OP_UNKNWN:
            std::cerr << "UNKNOWN VM OPERAND" << std::endl;
            break;
        default:
            std::cerr << "UNHANDLED VM STATE" << std::endl;
        }
        return RS_CMD;
    default:
        {
            zhvm::cmd zcmd = {0};

            const char *result = Assemble(input.c_str(), &zcmd);

            if (result == 0) {
                std::cerr << "Bad instruction: " << input << std::endl;
                return RS_NEXT;
            }

            if (result != 0) {
                if (strlen(result)!=0){
                    std::cout << result << std::endl;
                }
                std::cout << std::hex << "0x" << zhvm::AsBytes(zcmd) << std::endl;
            }

            switch (zhvm::Invoke(mem, zcmd)){
            case zhvm::IR_HALT:
                std::cout << "HALT VM" << std::endl;
                return RS_BREAK;
            case zhvm::IR_OP_UNKNWN:
                std::cerr << "UNKNOWN VM OPERAND" << std::endl;
                return RS_BREAK;
            case zhvm::IR_RUN:
                break;
            }
        }
    }
    return RS_NEXT;
}

int main() {

    PrintIntro();

    zhvm::memory mem;

    bool loop = true;
    while (loop) {
        if (std::cin.good()) {
            std::cout << "-> ";
            
            switch (replRound(std::cin, &mem)){
            case RS_BREAK:
                loop = false;
                continue;
            case RS_CMD:
                break;
            case RS_NEXT:
                mem.Print(std::cout);
                break;
            default:
                std::cerr << "BAD REPL STATE" << std::endl;
                loop = false;
                continue;
            }
        }
    }

    return 0;
}
