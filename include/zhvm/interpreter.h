/**
 * @file interpreter.h
 * @author marko
 *
 * Main ZHVM interpreter functions
 *
 */

#ifndef __INTERPRETER_HEADER__
#define __INTERPRETER_HEADER__

#include <cstdint>

#include "constants.h"
#include "memory.class.h"

namespace zhvm {

    /**
     * Invoke specified command on VM memory.
     *
     * @param mem VM memory
     * @param icmd VM command to process
     * @return invoke result
     * @see zhvm::invoke_result
     */
    int Invoke(memory *mem, uint32_t icmd);

    /**
     * Run programm in VM memory.
     * 
     * Fetch every instruction one by one from VM memory at $p offset
     * and then inteprets it. This aproach enables code self modifications. 
     *
     * @param mem VM memory
     * @return program execution result
     * @see zhvm::invoke_result
     */
    int Execute(memory* mem, bool debug);

    /**
     * 
     * Run program in VM memory cached instructions.
     * 
     * Instructions fetched in chunks from VM memory starting at $p offset up
     * until cache is filled or hit command which can modify $p register. After 
     * that all whole fetched chunk is interpteted. This aproach forbids code 
     * self modifications, but around 30% faster.
     * 
     * @param mem VM memory
     * @return program execution result
     * @see zhvm::invoke_result
     */
    int ExecutePrefetch(memory* mem);

    /**
     * Single step.
     * 
     * @param mem VM memory
     * @return step execution result
     * @see zhvm::invoke_result
     */
    int Step(memory* mem);

}

#endif // __INTERPRETER_HEADER__
