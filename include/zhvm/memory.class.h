/**
 * @file memory.class.h
 * @author marko
 *
 * ZHVM memory class
 *
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
      
        reg_t regs[RTOTAL];
        int32_t sflag;

        char* cdata;
        size_t csize;

        char* ddata;
        size_t dsize;

        cfunc funcs[ZHVM_CFUNC_ARRAY_SIZE];

    public:

        /**
         * Default constructor. Create VM with 1024 bytes of memory
         */
        explicit memory();

        /**
         * Constructor with memory
         * @param memsize total VM memory size
         */
        explicit memory(size_t codesize, size_t datasize);

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
         * Check and reset RP register set bit 
         */
        inline int32_t TestSetRP() {
            int32_t result = this->sflag & (1 << RP);
            this->sflag = 0;
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
         * 
         * Set code instruction in memory
         * 
         * @param offset memory offset
         * @param code code to set
         * @return self
         */
        memory& SetCode(off_t offset, uint32_t code);


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
         * Copy memory
         * 
         * @param dest destination offset
         * @param src source offset
         * @param len copy byte length
         * @return self
         */
        memory& Copy(off_t dest, off_t src, size_t len);

        /**
         * Compare memory
         * 
         * @param src0 pointer A
         * @param src1 pointer B
         * @param len data size to compare
         * @return zero if data is identical
         */
        int32_t Compare(off_t src0, off_t src1, size_t len);

        /**
         * 
         * Get code from memory.
         * 
         * @param offset memory offset
         * @return code 
         */
        inline uint32_t GetCode(off_t offset) const {
            if (offset + sizeof (uint32_t) < this->csize) {
                return *(uint32_t*) (this->cdata + offset);
            }
            throw std::runtime_error("Code Access Violation");
        }

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
         * Create new vm image
         */
        void NewImage(size_t codesize, size_t datasize);

        /**
         * Assign function to index
         */
        void SetFuncs(uint32_t index, cfunc func);

        /**
         * Call function by index
         */
        int Call(uint32_t index);

    };
    
    /**
     * 
     * Main hashing function.
     * 
     * @param hash old hash
     * @param data input data buffer
     * @param len input data length
     * @return new hash
     */
    uint32_t sdbm(uint32_t hash, const void* data, size_t len);


}

#endif // __ZMEM_CLASS_HEADER__

