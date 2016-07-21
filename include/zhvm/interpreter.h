/**
 * @file interpreter.h
 * @author marko
 */

#ifndef __INTERPRETER_HEADER__
#define __INTERPRETER_HEADER__

#include <cstdint>

#include "zconst.h"
#include "zmem.class.h"

namespace zhvm {

    /**
     * Main VM command structure.
     */
    struct cmd {
        uint32_t opc:6;  ///  6-BIT OPERATION CODE
        uint32_t dr:4;   ///  4-BIT DESTINATION REGISTER
        uint32_t ds0:4;  ///  4-BIT SOURCE 0 REGISTER
        uint32_t ds1:2;  ///  2-BIT SOURCE 1 REGISTER (RZ, RA, RB, RC)
        int16_t im:16;   /// 16-BIT INTERMEDIATE CONSTANT
    };

    /**
     * Convert command to 4 bytes.
     *
     * @param command command to convert
     * @return command as bytes
     */
    inline uint32_t AsBytes(cmd command){
        return *reinterpret_cast<uint32_t *>(&command);
    }

    /**
     * Convert bytes to command.
     *
     * @param bytes bytes to convert
     * @return bytes as command
     */
    inline cmd AsCommand(uint32_t bytes){
        return *reinterpret_cast<cmd *>(&bytes);
    }

    /**
     * Invoke specified command on VM memory
     *
     * @param mem VM memory
     * @param icmd VM command to process
     * @return invoke result
     * @see invoke_result
     */
    int Invoke(memory* mem, cmd icmd);

    /**
     * Run programm in VM memory
     *
     * @param mem VM memory
     * @return program execution result
     * @see invoke_result
     */
    int Execute(memory* mem);

}

#endif // __INTERPRETER_HEADER__
