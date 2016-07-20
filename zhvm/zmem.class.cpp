#include <iomanip>
#include <fstream>

#include "zhvm.h"
#include "sman.h"
#include "zmem.class.h"

namespace zhvm {

    memory::memory(size_t memsize):regs(),mdata(0),msize(0){
        this->mdata = new char[memsize];
        this->msize = memsize;
    }

    memory::~memory(){
        delete[] this->mdata;
    }

    memory& memory::Set(uint32_t reg, int64_t val){
        if ((reg > RZ)&&(reg < RTOTAL)){
            this->regs[reg-1] = val;
        }
        return *this;
    }

    int64_t memory::Get(uint32_t reg){
        if ((reg > RZ)&&(reg < RTOTAL)){
            return this->regs[reg-1];
        }
        return 0;
    }

    memory& memory::SetByte(uint32_t offset, int64_t val){
        if (this->mdata != 0){
            *(int8_t*)(this->mdata+offset) = (int8_t)val;
        }
        return *this;
    }

    memory& memory::SetShort(uint32_t offset, int64_t val){
        if (this->mdata != 0){
            *(int16_t*)(this->mdata+offset) = (int16_t)val;
        }
        return *this;
    }

    memory& memory::SetLong(uint32_t offset, int64_t val){
        if (this->mdata != 0){
            *(int32_t*)(this->mdata+offset) = (int32_t)val;
        }
        return *this;
    }

    memory& memory::SetQuad(uint32_t offset, int64_t val){
        if (this->mdata != 0){
            *(int64_t*)(this->mdata+offset) = val;
        }
        return *this;
    }

    int8_t memory::GetByte(uint32_t offset) {
        if (this->mdata != 0) {
            return *(int8_t*)(this->mdata+offset);
        }
        return 0;
    }

    int16_t memory::GetShort(uint32_t offset) {
        if (this->mdata != 0) {
            return *(int16_t*)(this->mdata+offset);
        }
        return 0;
    }

    int32_t memory::GetLong(uint32_t offset) {
        if (this->mdata != 0) {
            return *(int32_t*)(this->mdata+offset);
        }
        return 0;
    }

    int64_t memory::GetQuad(uint32_t offset) {
        if (this->mdata != 0) {
            return *(int64_t*)(this->mdata+offset);
        }
        return 0;
    }

    void memory::Print(std::ostream& output){
        for (int i = RA; i < RTOTAL; ++i){
            output <<  GetRegisterName(i)<< ": " << std::setw(17) << std::hex << this->Get(i) << std::endl;
        }
    }

    void memory::Dump(){
        std::ofstream dumpfile("dump.bin", std::ios_base::out | std::ios_base::binary);
        if (dumpfile){
            dumpfile.write(this->mdata, this->msize);
        }
        dumpfile.close();
    }

    void memory::Load(){
        std::ifstream loadfile("dump.bin", std::ios_base::in | std::ios_base::binary);
        if (loadfile){
            loadfile.read(this->mdata, this->msize);
        }
        loadfile.close();
    }

}
