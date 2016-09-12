#include <iomanip>
#include <fstream>
#include <iostream>
#include <cstring>

#include <zhvm.h>
#include <string.h>

namespace zhvm {

    int none(memory* mem) {
        return IR_HALT;
    }

    memory::memory() : regs(), sflag(0), cdata(0), csize(0), ddata(0), dsize(0), funcs() {
        this->NewImage(1024, 1024);
    }

    memory::memory(size_t codesize, size_t datasize) : regs(), sflag(0), cdata(0), csize(0), ddata(0), dsize(0), funcs() {
        this->NewImage(codesize, datasize);
    }

    memory::memory(const memory& copy) : regs(), sflag(copy.sflag), cdata(0), csize(0), ddata(0), dsize(0), funcs() {
        this->cdata = new char[copy.csize];
        this->csize = copy.csize;
        memcpy(this->cdata, copy.cdata, this->csize);


        this->ddata = new char[copy.dsize];
        this->dsize = copy.dsize;
        memcpy(this->ddata, copy.ddata, this->dsize);

        for (int i = RZ; i < RTOTAL; ++i) {
            this->regs[i] = copy.regs[i];
        }

        for (uint32_t i = 0; i < ZHVM_CFUNC_ARRAY_SIZE; ++i) {
            this->funcs[i] = copy.funcs[i];
        }

    }

    memory& memory::operator=(const memory& src) {
        if (this != &src) {
            delete[] this->cdata;

            this->cdata = new char[src.csize];
            this->csize = src.csize;
            memcpy(this->cdata, src.cdata, this->csize);

            delete[] this->ddata;

            this->ddata = new char[src.dsize];
            this->dsize = src.dsize;
            memcpy(this->ddata, src.ddata, this->dsize);

            for (int i = RZ; i < RTOTAL; ++i) {
                this->regs[i] = src.regs[i];
            }
            for (uint32_t i = 0; i < ZHVM_CFUNC_ARRAY_SIZE; ++i) {
                this->funcs[i] = src.funcs[i];
            }
            this->sflag = src.sflag;
        }
        return *this;
    }

    memory& memory::operator=(memory&& src) {
        if (this != &src) {
            delete[] this->cdata;

            this->cdata = src.cdata;
            this->csize = src.csize;

            delete[] this->ddata;

            this->ddata = src.ddata;
            this->dsize = src.dsize;

            for (int i = RZ; i < RTOTAL; ++i) {
                this->regs[i] = src.regs[i];
            }
            for (uint32_t i = 0; i < ZHVM_CFUNC_ARRAY_SIZE; ++i) {
                this->funcs[i] = src.funcs[i];
            }
            this->sflag = src.sflag;

            src.cdata = 0;
            src.csize = 0;

            src.ddata = 0;
            src.dsize = 0;
        }
        return *this;
    }

    memory::memory(memory&& mv) : regs(), sflag(mv.sflag), cdata(mv.cdata), csize(mv.csize), ddata(mv.ddata), dsize(mv.dsize), funcs() {
        for (int i = RZ; i < RTOTAL; ++i) {
            this->regs[i] = mv.regs[i];
        }
        for (uint32_t i = 0; i < ZHVM_CFUNC_ARRAY_SIZE; ++i) {
            this->funcs[i] = mv.funcs[i];
        }
        mv.cdata = 0;
        mv.csize = 0;

        mv.ddata = 0;
        mv.dsize = 0;
    }

    memory::~memory() {
        delete[] this->cdata;
        delete[] this->ddata;
    }

    memory& memory::SetCode(off_t offset, uint32_t val) {
        if (offset + sizeof (uint32_t) < this->csize) {
            *(uint32_t*) (this->cdata + offset) = (uint32_t) val;
            return *this;
        }
        std::cerr << "SetCode: " << std::hex << offset << " = " << std::dec << val << std::endl;
        throw std::runtime_error("Code Access Violation (SetCode)");
    }

    memory& memory::SetByte(off_t offset, int64_t val) {
        if (offset + sizeof (int8_t) < this->dsize) {
            *(int8_t*) (this->ddata + offset) = (int8_t) val;
            return *this;
        }
        std::cerr << "SetByte: " << std::hex << offset << " = " << std::dec << val << std::endl;
        throw std::runtime_error("Data Access Violation (SetByte)");
    }

    memory& memory::SetShort(off_t offset, int64_t val) {
        if (offset + sizeof (int16_t) < this->dsize) {
            *(int16_t*) (this->ddata + offset) = (int16_t) val;
            return *this;
        }
        std::cerr << "SetShort: " << std::hex << offset << " = " << std::dec << val << std::endl;
        throw std::runtime_error("Data Access Violation (SetShort)");
    }

    memory& memory::SetLong(off_t offset, int64_t val) {
        if (offset + sizeof (int32_t) < this->dsize) {
            *(int32_t*) (this->ddata + offset) = (int32_t) val;
            return *this;
        }
        std::cerr << "SetLong: " << std::hex << offset << " = " << std::dec << val << std::endl;
        throw std::runtime_error("Data Access Violation (SetLong)");
    }

    memory& memory::SetQuad(off_t offset, int64_t val) {
        if (offset + sizeof (int64_t) < this->dsize) {
            *(int64_t*) (this->ddata + offset) = val;
            return *this;
        }
        std::cerr << "SetQuad: " << std::hex << offset << " = " << std::dec << val << std::endl;
        throw std::runtime_error("Data Access Violation (SetQuad)");
    }

    memory & memory::Copy(off_t dest, off_t src, size_t len) {
        len = std::min<size_t>(len, this->dsize - dest);
        len = std::min<size_t>(len, this->dsize - src);

        memmove(this->ddata + dest, this->ddata + src, len);
        return *this;
    }

    int32_t memory::Compare(off_t src0, off_t src1, size_t len) {
        len = std::min<size_t>(len, this->dsize - src0);
        len = std::min<size_t>(len, this->dsize - src1);

        return memcmp(this->ddata + src0, this->ddata + src1, len);
    }

    int8_t memory::GetByte(off_t offset) const {
        if (offset + sizeof (int8_t) < this->dsize) {
            return *(int8_t*) (this->ddata + offset);
        }
        std::cerr << "GetByte: " << std::hex << offset << std::endl;
        throw std::runtime_error("Data Access Violation  (GetByte)");
    }

    int16_t memory::GetShort(off_t offset) const {
        if (offset + sizeof (int16_t) < this->dsize) {
            return *(int16_t*) (this->ddata + offset);
        }
        std::cerr << "GetShort: " << std::hex << offset << std::endl;
        throw std::runtime_error("Data Access Violation (GetShort)");
    }

    int32_t memory::GetLong(off_t offset) const {
        if (offset + sizeof (int32_t) < this->dsize) {
            return *(int32_t*) (this->ddata + offset);
        }
        std::cerr << "GetLong: " << std::hex << offset << std::endl;
        throw std::runtime_error("Data Access Violation (GetLong)");
    }

    int64_t memory::GetQuad(off_t offset) const {
        if (offset + sizeof (int64_t) < this->dsize) {
            return *(int64_t*) (this->ddata + offset);
        }
        std::cerr << "GetQuad: " << std::hex << offset << std::endl;
        throw std::runtime_error("Data Access Violation (GetQuad)");
    }

    void memory::Print(std::ostream & output) const {
        for (uint32_t i = RA; i < RTOTAL; ++i) {
            output << GetRegisterName(i) << ": " << std::setw(17) << std::hex << this->Get(i) << std::endl;
        }
    }

    struct memory_file_header {
        uint32_t magic;
        uint32_t version;
        uint32_t csize;
        uint32_t dsize;
        reg_t regs[RTOTAL - 1];
        int32_t sflag;
    };

    uint32_t sdbm(uint32_t hash, const void* data, size_t len) {
        const uint8_t* str = (const uint8_t*) data;
        while (len-- > 0) {
            hash = *str + (hash << 6) + (hash << 16) - hash;
            ++str;
        }
        return hash;
    }

    void memory::Dump(std::ostream & out) const {
        if (out) {
            memory_file_header mfh;
            mfh.magic = ZHVM_MEMORY_FILE_MAGIC;
            mfh.version = ZHVM_VM_VERSION;
            mfh.csize = this->csize;
            mfh.dsize = this->dsize;

            uint32_t hash = sdbm(0, &mfh, sizeof (memory_file_header));
            hash = sdbm(hash, this->cdata, this->csize);
            hash = sdbm(hash, this->ddata, this->dsize);
            hash = sdbm(hash, this->regs + 1, sizeof (reg_t)*(RTOTAL - 1));
            hash = sdbm(hash, &this->sflag, sizeof (int32_t));

            out.write((char*) &mfh, sizeof (memory_file_header));
            out.write(this->cdata, this->csize);
            out.write(this->ddata, this->dsize);
            out.write((char*) (this->regs + 1), sizeof (reg_t)*(RTOTAL - 1));
            out.write((char*) &this->sflag, sizeof (int32_t));
            out.write((char*) &hash, sizeof (uint32_t));
        }
    }

    void memory::Load(std::istream & inp) {
        if (inp) {
            memory_file_header mfh;
            inp.read((char*) &mfh, sizeof (memory_file_header));
            if (inp.gcount() != sizeof (memory_file_header)) {
                throw std::runtime_error("Unexpected EOF");
            }

            if (mfh.magic != ZHVM_MEMORY_FILE_MAGIC) {
                throw std::runtime_error("Not a ZHVM image");
            }

            if (mfh.version != ZHVM_VM_VERSION) {
                throw std::runtime_error("Invalid ZHVM version");
            }

            memory temp;
            temp.NewImage(mfh.csize, mfh.dsize);
            inp.read(temp.cdata, temp.csize);
            if (inp.gcount() != temp.csize) {
                throw std::runtime_error("Unexpected EOF");
            }

            inp.read(temp.ddata, temp.dsize);
            if (inp.gcount() != temp.dsize) {
                throw std::runtime_error("Unexpected EOF");
            }

            inp.read((char*)(temp.regs + 1), sizeof (reg_t)*(RTOTAL - 1));
            if (inp.gcount() != sizeof (reg_t)*(RTOTAL - 1)) {
                throw std::runtime_error("Unexpected EOF");
            }

            inp.read((char*) &temp.sflag, sizeof (int32_t));
            if (inp.gcount() != sizeof (int32_t)) {
                throw std::runtime_error("Unexpected EOF");
            }

            uint32_t hash = sdbm(0, &mfh, sizeof (memory_file_header));
            hash = sdbm(hash, temp.cdata, temp.csize);
            hash = sdbm(hash, temp.ddata, temp.dsize);
            hash = sdbm(hash, temp.regs + 1, sizeof (reg_t)*(RTOTAL - 1));
            hash = sdbm(hash, &temp.sflag, sizeof (int32_t));

            uint32_t fhash = 0;
            inp.read((char*) &fhash, sizeof (uint32_t));
            if (inp.gcount() != sizeof (uint32_t)) {
                throw std::runtime_error("Unexpected EOF");
            }

            if (fhash != hash) {
                throw std::runtime_error("ZHVM image corrupted");
            }
            *this = std::move(temp);
        }
    }

    void memory::SetFuncs(uint32_t index, cfunc funcs) {
        this->funcs[index] = funcs;
    }

    int memory::Call(uint32_t index) {
        return this->funcs[index](this);
    }

    void memory::NewImage(size_t codesize, size_t datasize) {
        delete[] this->cdata;
        delete[] this->ddata;

        this->cdata = new char[codesize];
        this->csize = codesize;

        this->ddata = new char[datasize];
        this->dsize = datasize;

        for (int i = RZ; i < RTOTAL; ++i) {
            this->regs[i] = 0;
        }

        for (size_t i = 0; i < ZHVM_CFUNC_ARRAY_SIZE; ++i) {
            this->funcs[i] = none;
        }
    }


}
