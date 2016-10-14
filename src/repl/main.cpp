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
#include "ast.class.h"

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
    RC_STEP, ///< Do one step
    RC_BURST, ///< Burst program execution from current RP position
    RC_NOLOG, ///< Disable log
    RC_LOG, ///< Enable log
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
        "  ~step  - make one program step in vm memory from $p offset",
        "  ~burst - burst program execution in vm memory from $p offset",
        "  ~nolog - disable log",
        "  ~log   - enable log",
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

        "   gr [0x13] D = S0 > (S1 + IM)",
        "   ls [0x14] D = S0 < (S1 + IM)",
        "  gre [0x15] D = S0 >= (S1 + IM)",
        "  lse [0x16] D = S0 <= (S1 + IM)",
        "   eq [0x17] D = S0 == (S1 + IM)",
        "  neq [0x18] D = S0 != (S1 + IM)",

        "  cll [0x19] CALL C FUNCTION",
        "  cpy [0x1A] mempy(D, S0, S1 + IM)",
        "  cmp [0x1B] D = memcmp(D, S0, S1 + IM)",
        "  zcl [0x1C] CALL ZHVM FUNCTION",
        "  ret [0x1D] RETURN FROM ZHVM FUNCTION",
        "  not [0x1E] D = !(S0 | (S1 + IM))"
        "  nop [0x3F] DO NOTHING",
        0
    };

    int regprinter = 0;

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
        "~exit\n",
        "~intro\n",
        "~list\n",
        "~dump\n",
        "~load\n",
        "~exec\n",
        "~reset\n",
        "~step\n",
        "~burst\n",
        "~nolog\n",
        "~log\n",
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
    RS_CMD = 2, ///< REPL command processed
    RS_NONE = 3 ///< Empty line
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

    while (true) {
        std::string inputline;

        std::getline(istrm, inputline);
        if (inputline.length() == 0) {
            break;
        }

        if (inputline.back() == '\\') {
            inputline.pop_back();
            input.append(inputline);
            input.append("\n");
        } else {
            input.append(inputline);
            input.append("\n");
            break;
        }
    }

    if (input.length() == 0) {
        return RS_NONE;
    }

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
                    if (regprinter) {
                        std::cout << "HALT VM" << std::endl;
                    }
                    break;
                case zhvm::IR_OP_UNKNWN:
                    std::cerr << "UNKNOWN VM OPERAND" << std::endl;
                    break;
                default:
                    std::cerr << "UNHANDLED VM STATE" << std::endl;
            }

            if (regprinter) {
                mem->Print(std::cout);
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
        case RC_STEP:
        {
            zhvm::TD_TIME start;
            zhvm::TD_TIME stop;

            zhvm::zhtime(&start);
            int result = zhvm::Step(mem);
            zhvm::zhtime(&stop);
            std::cout << "EXECUTION TIME: " << zhvm::time_diff(start, stop) << " SEC" << std::endl;
            switch (result) {
                case zhvm::IR_RUN:
                    if (regprinter) {
                        std::cout << "PROCEED" << std::endl;
                    }
                    break;
                case zhvm::IR_HALT:
                    if (regprinter) {
                        std::cout << "HALT VM" << std::endl;
                    }
                    break;
                case zhvm::IR_OP_UNKNWN:
                    std::cerr << "UNKNOWN VM OPERAND" << std::endl;
                    break;
                default:
                    std::cerr << "UNHANDLED VM STATE" << std::endl;
            }
            if (regprinter) {
                mem->Print(std::cout);
            }
            return RS_CMD;
        }
        case RC_BURST:
        {
            zhvm::TD_TIME start;
            zhvm::TD_TIME stop;

            zhvm::zhtime(&start);
            int result = zhvm::ExecutePrefetch(mem);
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
            if (regprinter) {
                mem->Print(std::cout);
            }
            return RS_CMD;
        }
        case RC_NOLOG:
            regprinter = 0;
            return RS_CMD;
        case RC_LOG:
            regprinter = 1;
            return RS_CMD;
        case RC_TOTAL:
            std::cerr << "UNKNOWN REPL COMMAND: " << input << std::endl;
            return RS_CMD;
        default:
        {
            std::stringstream pinput;
            zlg::context cont;
            zlg::ast tree;

            try {
                tree.Scan(input.c_str());
                tree.Generate(pinput, &cont, 0);
            } catch (std::runtime_error& err) {
                std::cerr << "ZLG ERROR: " << err.what() << std::endl;
                return RS_NEXT;
            }

            if (zhvm::Assemble(pinput.str().c_str(), mem, zhvm::LL_ERROR) == 0) {
                std::cerr << "BAD INSTRUCTION: " << input << std::endl;
                return RS_NEXT;
            }

            if (input[0] != '!') {
                int result = zhvm::IR_RUN;
                while (result == zhvm::IR_RUN) {
                    result = zhvm::Execute(mem);
                    switch (result) {
                        case zhvm::IR_HALT:
                            if (regprinter) {
                                std::cout << "HALT VM" << std::endl;
                            }
                            return RS_NEXT;
                        case zhvm::IR_OP_UNKNWN:
                            std::cerr << "UNKNOWN VM OPERAND" << std::endl;
                            return RS_BREAK;
                        default:
                            std::cerr << "UNHANDLED VM STATE" << std::endl;
                    }
                }
            }
        }
    }
    return RS_NEXT;
}

int vm_put(zhvm::memory * mem) {
    std::cout << std::dec << mem->Get(zhvm::RA) << std::endl;
    return zhvm::IR_RUN;
}

int vm_get(zhvm::memory * mem) {
    int val = 0;
    std::cin >> val;
    mem->Set(zhvm::RA, val);
    return zhvm::IR_RUN;
}

int vm_putc(zhvm::memory * mem) {
    std::cout << (char) mem->Get(zhvm::RA) << std::endl;
    return zhvm::IR_RUN;
}

int vm_getc(zhvm::memory * mem) {
    char val = 0;
    std::cin >> val;
    mem->Set(zhvm::RA, val);
    return zhvm::IR_RUN;
}

/**
 * REPL application entry point
 */
int main() {

    PrintIntro();

    zhvm::memory mem;
    mem.SetFuncs(zhvm::CN_PUT, vm_put);
    mem.SetFuncs(zhvm::CN_GET, vm_get);
    mem.SetFuncs(zhvm::CN_PUTC, vm_putc);
    mem.SetFuncs(zhvm::CN_GETC, vm_getc);

    bool loop = true;
    int replstat = RS_BREAK;
    while (loop) {
        if (std::cin.good()) {

            if (replstat != RS_NONE) {
                std::cout << "-> ";
            }

            replstat = replRound(std::cin, &mem);

            switch (replstat) {
                case RS_BREAK:
                    loop = false;
                    continue;
                case RS_CMD:
                    break;
                case RS_NEXT:
                    if (regprinter) {
                        mem.Print(std::cout);
                    }
                    break;
                case RS_NONE:
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
