#include <iomanip>
#include <fstream>
#include <cstring>

#include <zhvm.h>

namespace zhvm {

    memory::memory(size_t memsize) : regs(), mdata(0), msize(0) {
        this->mdata = new char[memsize];
        this->msize = memsize;
        for (int i = RZ; i < RTOTAL; ++i) {
            this->regs[i] = 0;
        }
    }

    memory::memory(const memory& copy) : regs(), mdata(0), msize(0) {
        this->mdata = new char[copy.msize];
        this->msize = copy.msize;
        memcpy(this->mdata, copy.mdata, this->msize);

        for (int i = RZ; i < RTOTAL; ++i) {
            this->regs[i] = copy.regs[i];
        }
    }

    memory::memory(memory&& mv) : regs(), mdata(mv.mdata), msize(mv.msize) {
        for (int i = RZ; i < RTOTAL; ++i) {
            this->regs[i] = mv.regs[i];
        }
        mv.mdata = 0;
        mv.msize = 0;
    }

    memory::~memory() {
        delete[] this->mdata;
    }

    memory& memory::SetByte(uint32_t offset, int64_t val) {
        *(int8_t*) (this->mdata + offset) = (int8_t) val;
        return *this;
    }

    memory& memory::SetShort(uint32_t offset, int64_t val) {
        *(int16_t*) (this->mdata + offset) = (int16_t) val;
        return *this;
    }

    memory& memory::SetLong(uint32_t offset, int64_t val) {
        *(int32_t*) (this->mdata + offset) = (int32_t) val;
        return *this;
    }

    memory& memory::SetQuad(uint32_t offset, int64_t val) {
        *(int64_t*) (this->mdata + offset) = val;
        return *this;
    }

    int8_t memory::GetByte(uint32_t offset) const {
        return *(int8_t*) (this->mdata + offset);
    }

    int16_t memory::GetShort(uint32_t offset) const {
        return *(int16_t*) (this->mdata + offset);
    }

    int32_t memory::GetLong(uint32_t offset) const {
        return *(int32_t*) (this->mdata + offset);
    }

    int64_t memory::GetQuad(uint32_t offset) const {
        return *(int64_t*) (this->mdata + offset);
    }

    void memory::Print(std::ostream& output) const {
        for (uint32_t i = RA; i < RTOTAL; ++i) {
            output << GetRegisterName(i) << ": " << std::setw(17) << std::hex << this->Get(i) << std::endl;
        }
    }

    void memory::Dump() const {
        std::ofstream dumpfile("dump.bin", std::ios_base::out | std::ios_base::binary);
        if (dumpfile) {
            dumpfile.write(this->mdata, this->msize);
        }
        dumpfile.close();
    }

    void memory::Load() {
        std::ifstream loadfile("dump.bin", std::ios_base::in | std::ios_base::binary);
        if (loadfile) {
            loadfile.read(this->mdata, this->msize);
        }
        loadfile.close();
    }

}
