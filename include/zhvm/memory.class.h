/**
 * @file zmem.class.h
 * @author marko
 */

#pragma once
#ifndef __ZMEM_CLASS_HEADER__
#define __ZMEM_CLASS_HEADER__

#include <ostream>

namespace zhvm {

    /**
     * VM memory class.
     */
    class memory {
        int64_t regs[RTOTAL];
        char* mdata;
        size_t msize;

    public:

        /**
         * Default constructor.
         * @param memsize total VM memory size
         */
        explicit memory(size_t memsize = 1024);

        /**
         * Copy constructor
         * @param copy memory copy
         */
        explicit memory(const memory& copy);

        /**
         * Move constructor
         * @param mv rhs object
         */
        explicit memory(memory&& mv);

        /**
         * Destructor.
         */
        ~memory();

        /**
         * Set register value.
         *
         * @param reg register ID
         * @param val new register value
         * @return self
         * @see registers
         */
        inline memory& Set(uint32_t reg, int64_t val) {
            if (reg != RZ) {
                this->regs[reg] = val;
            }
            return *this;
        }

        /**
         * Get register value.
         *
         * @param reg register ID
         * @return current register value
         * @see registers
         */
        inline int64_t Get(uint32_t reg) const {
            return this->regs[reg];
        }


        /**
         * Set byte in memory.
         *
         * @param offset memory offset
         * @param val value to set
         * @return self
         */
        memory& SetByte(uint32_t offset, int64_t val);

        /**
         * Set short in memory.
         *
         * @param offset memory offset
         * @param val value to set
         * @return self
         */
        memory& SetShort(uint32_t offset, int64_t val);

        /**
         * Set long in memory.
         *
         * @param offset memory offset
         * @param val value to set
         * @return self
         */
        memory& SetLong(uint32_t offset, int64_t val);

        /**
         * Set quad in memory.
         *
         * @param offset memory offset
         * @param val value to set
         * @return self
         */
        memory& SetQuad(uint32_t offset, int64_t val);

        /**
         * Get byte from memory.
         *
         * @param offset memory offset
         * @return byte value
         */
        int8_t GetByte(uint32_t offset) const;

        /**
         * Get short from memory.
         *
         * @param offset memory offset
         * @return short value
         */
        int16_t GetShort(uint32_t offset) const;

        /**
         * Get long from memory.
         *
         * @param offset memory offset
         * @return long value
         */
        int32_t GetLong(uint32_t offset) const;

        /**
         * Get quad from memory.
         *
         * @param offset memory offset
         * @return quad value
         */
        int64_t GetQuad(uint32_t offset) const;

        /**
         * Print registers state to stream.
         *
         * @param output output stream
         */
        void Print(std::ostream& output) const;

        /**
         * Dump VM memory image to file "dump.bin"
         */
        void Dump() const;

        /**
         * Load VM memory image from file "dump.bin"
         */
        void Load();

    };

}

#endif // __ZMEM_CLASS_HEADER__

