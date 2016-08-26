/**
 * @file main.cpp
 * @author marko
 *
 * ZHVM Read-Eval-Print-Loop application.
 *
 */

#include <iostream>
#include <string>
#include <cstring>
#include <ctime>

#include <zhvm.h>
#include <fstream>

#include "zhtime.h"

/**
 * List of avilable REPL commands
 */
enum replcmd {
    RC_NONE = 0, ///< Input is not a command, but VM instructure
    RC_EXIT, ///< Exit REPL
    RC_INTRO, ///< Print intro text
    RC_LIST, ///< Print VM opcodes list
    RC_DUMP, ///< Dump current VM memory to file "dump.bin"
    RC_LOAD, ///< Load VM memory from "dump.bin"
    RC_EXEC, ///< Start program execution from current RP position
    RC_RESET, ///< Set all registers to zero
    RC_TOTAL ///< Total REPL command count
};

namespace {

    /**
     * Commands list, Intro text.
     */
    const char* cmdlist[] = {
        "  ~exit  - close interpreter.",
        "  ~intro - show intro (this text).",
        "  ~list  - show operand list.",
        "  ~dump  - dump current vm memory to file \"dump.bin\".",
        "  ~load  - load file \"dump.bin\" to vm memory.",
        "  ~exec  - execute program in vm memory from $p offset.",
        "  ~reset - reset all registers to zero",
        0
    };

    /**
     * VM operand list and its description.
     */
    const char* oplist[] = {
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
        "  nop [0x3F] DO NOTHING",
        0
    };

}

/**
 * Parse command from input string and return replcmd.
 *
 * @param str input string
 * @return REPL command ID.
 * @see replcmd
 */
int GetCMD(const std::string &str) {

    static const char* replstr[] = {
        "~exit",
        "~intro",
        "~list",
        "~dump",
        "~load",
        "~exec",
        "~reset",
        0
    };

    if (str[0] == '~') {
        const char** cursor = replstr;
        int index = RC_NONE;
        while (*(cursor + index) != 0) {
            if (str.compare(*(cursor + index)) == 0) {
                return index + 1;
            }
            ++index;
        }
        return RC_TOTAL;
    }

    return RC_NONE;
}

/**
 * Print welcome text and string list line by line.
 *
 * @param welcome welcome text
 * @param list string list
 */
void PrintWelcomeList(const char* welcome, const char** list) {
    std::cout << welcome << std::endl;
    const char** cursor = list;
    while (*cursor != 0) {
        std::cout << *cursor << std::endl;
        ++cursor;
    }
}

/**
 * Print commands list (intro text).
 */
void PrintIntro() {
    PrintWelcomeList("Welcome!\nUsage:", cmdlist);
}

/**
 * Print avilable VM operand list and its description.
 */
void PrintList() {
    PrintWelcomeList("ZHVM operand list:", oplist);
}

/**
 * REPL round state
 */
enum repl_state {
    RS_BREAK = 0, ///< Break execution
    RS_NEXT = 1, ///< Wait next command
    RS_CMD = 2 ///< REPL command processed
};

/**
 * Read line from input stream and evaluate it.
 *
 * @param istrm input character stream
 * @param mem VM memory
 * @retval RS_BREAK if execution end reached,
 * @retval RS_NEXT if waits for next command,
 * @retval RS_CMD if REPL cmd was processed.
 * @see repl_state
 */
int replRound(std::istream& istrm, zhvm::memory* mem) {

    std::string input;
    std::getline(istrm, input);

    int cmd = GetCMD(input);

    switch (cmd) {
        case RC_EXIT:
            return RS_BREAK;
        case RC_INTRO:
            PrintIntro();
            return RS_CMD;
        case RC_LIST:
            PrintList();
            return RS_CMD;
        case RC_DUMP:
        {
            std::ofstream out("dump.bin", std::ios_base::out | std::ios_base::binary);
            mem->Dump(out);
            out.close();
            return RS_CMD;
        }
        case RC_LOAD:
        {
            std::ifstream inp("dump.bin", std::ios_base::in | std::ios_base::binary);
            mem->Load(inp);
            inp.close();
            return RS_CMD;
        }
        case RC_EXEC:
        {
            zhvm::TD_TIME start;
            zhvm::TD_TIME stop;

            zhvm::zhtime(&start);
            int result = zhvm::Execute(mem);
            zhvm::zhtime(&stop);
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
            return RS_CMD;
        }
        case RC_RESET:
            mem->Set(zhvm::RA, 0);
            mem->Set(zhvm::RB, 0);
            mem->Set(zhvm::RC, 0);
            mem->Set(zhvm::R0, 0);
            mem->Set(zhvm::R1, 0);
            mem->Set(zhvm::R2, 0);
            mem->Set(zhvm::R3, 0);
            mem->Set(zhvm::R4, 0);
            mem->Set(zhvm::R5, 0);
            mem->Set(zhvm::R6, 0);
            mem->Set(zhvm::R7, 0);
            mem->Set(zhvm::R8, 0);
            mem->Set(zhvm::RS, 0);
            mem->Set(zhvm::RD, 0);
            mem->Set(zhvm::RP, 0);
            return RS_CMD;
        case RC_TOTAL:
            std::cerr << "UNKNOWN REPL COMMAND: " << input << std::endl;
            return RS_CMD;
        default:
        {

            if (zhvm::Assemble(input.c_str(), mem) == 0) {
                std::cerr << "BAD INSTRUCTION: " << input << std::endl;
                return RS_NEXT;
            }

            if (input[0] != '!') {
                switch (zhvm::Step(mem)) {
                    case zhvm::IR_HALT:
                        std::cout << "HALT VM" << std::endl;
                        return RS_BREAK;
                    case zhvm::IR_OP_UNKNWN:
                        std::cerr << "UNKNOWN VM OPERAND" << std::endl;
                        return RS_BREAK;
                    case zhvm::IR_RUN:
                        break;
                    default:
                        std::cerr << "UNHANDLED VM STATE" << std::endl;
                }
            } else {

            }
        }
    }
    return RS_NEXT;
}

/**
 * REPL application entry point
 */
int main() {

    PrintIntro();

    zhvm::memory mem;

    bool loop = true;
    while (loop) {
        if (std::cin.good()) {
            std::cout << "-> ";

            switch (replRound(std::cin, &mem)) {
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
