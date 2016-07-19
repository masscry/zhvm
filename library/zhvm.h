//
// Created by timur on 12.07.16.
//

#ifndef ZHVM_ZHVM_H
#define ZHVM_ZHVM_H

#include <cstdint>
#include <ostream>
#include <iomanip>
#include <fstream>

namespace zhvm {

    enum opcodes {
        OP_UNKNOWN = -1,
        OP_HALT = 0, // 0x00 HALT VM
        OP_ADD,      // 0x01 D = S0 + (S1 + IM)
        OP_SUB,      // 0x02 D = S0 - (S1 + IM)
        OP_MUL,      // 0x03 D = S0 * (S1 + IM)
        OP_DIV,      // 0x04 D = S0 / (S1 + IM)
        OP_MOD,      // 0x05 D = S0 % (S1 + IM)
        OP_CMZ,      // 0x06 Conditional move zero
        OP_CMN,      // 0x07 Conditional move non-zero

        OP_LDB,      // 0x08 D = (1 byte)mem[S0+S1+IM]
        OP_LDS,      // 0x09 D = (2 bytes)mem[S0+S1+IM]
        OP_LDL,      // 0x0A D = (4 bytes)mem[S0+S1+IM]
        OP_LDQ,      // 0x0B D = (8 bytes)mem[S0+S1+IM]

        OP_SVB,      // 0x0C mem[D+S1+IM] = (1 byte)S0
        OP_SVS,      // 0x0D mem[D+S1+IM] = (2 bytes)S0
        OP_SVL,      // 0x0E mem[D+S1+IM] = (4 bytes)S0
        OP_SVQ,      // 0x0F mem[D+S1+IM] = (8 bytes)S0
        
        OP_AND,      // 0x10 D = S0 & (S1 + IM)
        OP_OR,       // 0x11 D = S0 | (S1 + IM)
        OP_XOR,      // 0x12 D = S0 ^ (S1 + IM)

        OP_TOTAL
    };

    enum registers {
        RUNKNWN = -2,
        RNOTREG = -1,
        RZ = 0x0,
        RA = 0x1,
        RB = 0x2,
        RC = 0x3,
        R0 = 0x4,
        R1 = 0x5,
        R2 = 0x6,
        R3 = 0x7,
        R4 = 0x8,
        R5 = 0x9,
        R6 = 0xA,
        R7 = 0xB,
        R8 = 0xC,
        RS = 0xD,
        RD = 0xE,
        RP = 0xF,
        RTOTAL = 0x10
    };

    const char* GetRegisterName(uint32_t reg);

    struct cmd {
        uint32_t opc:6;
        uint32_t dr:4;
        uint32_t ds0:4;
        uint32_t ds1:2;
        int16_t im:16;
    };
    
    union ucmd {
        int32_t i32;
        cmd c32;
    };

    const char *Assemble(const char *text, cmd *result);

    uint32_t AsBytes(cmd command);

    class memory {
        int64_t regs[RTOTAL-1];
        char* mdata;
        size_t msize;

    public:

        memory(size_t memsize = 1024):regs(),mdata(0),msize(0){
            this->mdata = new char[memsize];
            this->msize = memsize;
        }

        ~memory(){
            delete[] this->mdata;
        }

        memory& Set(uint32_t reg, int64_t val){
            if ((reg > RZ)&&(reg < RTOTAL)){
                this->regs[reg-1] = val;
            }
            return *this;
        }

        int64_t Get(uint32_t reg){
            if ((reg > RZ)&&(reg < RTOTAL)){
                return this->regs[reg-1];
            }
            return 0;
        }

        memory& SetByte(uint32_t offset, int64_t val){
            if (this->mdata != 0){
                *(int8_t*)(this->mdata+offset) = (int8_t)val;
            }
            return *this;
        }

        memory& SetShort(uint32_t offset, int64_t val){
            if (this->mdata != 0){
                *(int16_t*)(this->mdata+offset) = (int16_t)val;
            }
            return *this;
        }

        memory& SetLong(uint32_t offset, int64_t val){
            if (this->mdata != 0){
                *(int32_t*)(this->mdata+offset) = (int32_t)val;
            }
            return *this;
        }

        memory& SetQuad(uint32_t offset, int64_t val){
            if (this->mdata != 0){
                *(int64_t*)(this->mdata+offset) = val;
            }
            return *this;
        }

        int8_t GetByte(uint32_t offset) {
            if (this->mdata != 0) {
                return *(int8_t*)(this->mdata+offset);
            }
            return 0;
        }

        int16_t GetShort(uint32_t offset) {
            if (this->mdata != 0) {
                return *(int16_t*)(this->mdata+offset);
            }
            return 0;
        }

        int32_t GetLong(uint32_t offset) {
            if (this->mdata != 0) {
                return *(int32_t*)(this->mdata+offset);
            }
            return 0;
        }

        int64_t GetQuad(uint32_t offset) {
            if (this->mdata != 0) {
                return *(int64_t*)(this->mdata+offset);
            }
            return 0;
        }

        void Print(std::ostream& output){
            for (int i = RA; i < RTOTAL; ++i){
                output <<  GetRegisterName(i)<< ": " << std::setw(17) << std::hex << this->Get(i) << std::endl;
            }
        }
        
        void Dump(){
            std::ofstream dumpfile("dump.bin", std::ios_base::out | std::ios_base::binary);
            if (dumpfile){
                dumpfile.write(this->mdata, this->msize);
            }
            dumpfile.close();
        }
        
        void Load(){
           std::ifstream loadfile("dump.bin", std::ios_base::in | std::ios_base::binary);
           if (loadfile){
               loadfile.read(this->mdata, this->msize);
           }
           loadfile.close();
        }

    };



    enum invoke_result {
        IR_RUN = 0,
        IR_HALT = 1,
        IR_OP_UNKNWN = 2
    };

    int Invoke(memory* mem, cmd icmd);
    
    int Execute(memory* mem);

}

#endif //ZHVM_ZHVM_H
