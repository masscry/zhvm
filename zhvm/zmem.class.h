/**
 * @file zmem.class.h
 * @author marko
 */

#pragma once
#ifndef __ZMEM_CLASS_HEADER__
#define __ZMEM_CLASS_HEADER__

#include <ostream>

namespace zhvm {

    class memory {
        int64_t regs[RTOTAL-1];
        char* mdata;
        size_t msize;

    public:

        memory(size_t memsize = 1024);
        ~memory();

        memory& Set(uint32_t reg, int64_t val);
        int64_t Get(uint32_t reg);

        memory& SetByte(uint32_t offset, int64_t val);
        memory& SetShort(uint32_t offset, int64_t val);
        memory& SetLong(uint32_t offset, int64_t val);
        memory& SetQuad(uint32_t offset, int64_t val);

        int8_t GetByte(uint32_t offset);
        int16_t GetShort(uint32_t offset);
        int32_t GetLong(uint32_t offset);
        int64_t GetQuad(uint32_t offset);

        void Print(std::ostream& output);

        void Dump();
        void Load();

    };

}

#endif // __ZMEM_CLASS_HEADER__

