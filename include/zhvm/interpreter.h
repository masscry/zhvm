/**
 * @file interpreter.h
 * @author marko
 */

#ifndef __INTERPRETER_HEADER__
#define __INTERPRETER_HEADER__

#include <cstdint>

#include "constants.h"
#include "memory.class.h"

namespace zhvm {

    /**
     * Invoke specified command on VM memory
     *
     * @param mem VM memory
     * @param icmd VM command to process
     * @return invoke result
     * @see zhvm::invoke_result
     */
    int Invoke(memory *mem, uint32_t icmd);

    /**
     * Run programm in VM memory
     *
     * @param mem VM memory
     * @return program execution result
     * @see zhvm::invoke_result
     */
    int Execute(memory* mem);

    /**
     * Single step
     * 
     * @param mem VM memory
     * @return step execution result
     * @see zhvm::invoke_result
     */
    int Step(memory* mem);

}

#endif // __INTERPRETER_HEADER__
