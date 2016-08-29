/**
 * @file zmem.class.h
 * @author marko
 */

#pragma once
#ifndef __ZMEM_CLASS_HEADER__
#define __ZMEM_CLASS_HEADER__

#include <ostream>


namespace zhvm {

    class memory;

    /**
     * VM callback function
     */
    typedef int (*cfunc)(memory* mem);

    int none(memory* mem);

    /**
     * VM memory class.
     */
    class memory {
        int64_t regs[RTOTAL];
        int32_t sflag;
        char* mdata;
        size_t msize;

        cfunc funcs[ZHVM_CFUNC_ARRAY_SIZE];

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
         * Copy assignment
         */
        memory& operator=(const memory& src);

        /**
         * Move assignment
         */
        memory& operator=(memory&& src);


        /**
         * Destructor.
         */
        ~memory();

        /**
         * Inner function to reset set flag
         */
        inline void DropSet() {
            this->sflag = 0;
        }
        
        /**
         * Check and reset register set bit
         */
        inline int32_t TestSet(uint32_t reg) {
            int32_t result = this->sflag & (1 << reg);
            this->sflag &= ~(1 << reg);
            return result;
        }

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
            sflag |= (1 << reg);
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
        memory& SetByte(off_t offset, int64_t val);

        /**
         * Set short in memory.
         *
         * @param offset memory offset
         * @param val value to set
         * @return self
         */
        memory& SetShort(off_t offset, int64_t val);

        /**
         * Set long in memory.
         *
         * @param offset memory offset
         * @param val value to set
         * @return self
         */
        memory& SetLong(off_t offset, int64_t val);

        /**
         * Set quad in memory.
         *
         * @param offset memory offset
         * @param val value to set
         * @return self
         */
        memory& SetQuad(off_t offset, int64_t val);

        /**
         * Get byte from memory.
         *
         * @param offset memory offset
         * @return byte value
         */
        int8_t GetByte(off_t offset) const;

        /**
         * Get short from memory.
         *
         * @param offset memory offset
         * @return short value
         */
        int16_t GetShort(off_t offset) const;

        /**
         * Get long from memory.
         *
         * @param offset memory offset
         * @return long value
         */
        int32_t GetLong(off_t offset) const;

        /**
         * Get quad from memory.
         *
         * @param offset memory offset
         * @return quad value
         */
        int64_t GetQuad(off_t offset) const;

        /**
         * Print registers state to stream.
         *
         * @param output output stream
         */
        void Print(std::ostream& output) const;

        /**
         * Dump VM memory image to file "dump.bin"
         */
        void Dump(std::ostream& output) const;

        /**
         * Load VM memory image from file "dump.bin"
         */
        void Load(std::istream& input);

        /**
         * Assign function to index
         */
        void SetFuncs(uint32_t index, cfunc func);

        /**
         * Call function by index
         */
        int Call(uint32_t index);

    };

}

#endif // __ZMEM_CLASS_HEADER__

