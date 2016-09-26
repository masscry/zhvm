/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   context.h
 * Author: timur
 *
 * Created on 26 сентября 2016 г., 17:16
 */

#ifndef CONTEXT_H
#define CONTEXT_H

#include <unordered_map>
#include <string>
#include <cstring>

namespace zlg {

    enum regusage {
        RU_CONSTANT,
        RU_PREV_VAL,
        RU_VALUE
    };

    class context {
        uint32_t rlock[16];
    public:
        typedef std::unordered_map<std::string, uint32_t> usemap_t;
    private:
        usemap_t usemap;

    public:

        uint32_t GetFreeRegister(int usage, const char* id);
        uint32_t CountFreeRegsiters() const;

        bool CheckRegister(const char* id, uint32_t reg);
        void MarkRegister(int usage, const char* id, uint32_t reg);
        void ClearMap(uint32_t reg);

        void TotalReset();

        uint32_t RefCount(uint32_t reg) const;
        void AddRef(uint32_t reg);
        void Release(uint32_t reg);

        context() : rlock(), usemap() {
            ;
        }

        context(const context& src) : rlock(), usemap(src.usemap) {
            memcpy(this->rlock, src.rlock, sizeof (uint32_t)*16);
        }

        ~context() {
            ;
        }

        context& operator=(const context& src) {
            if (this != &src) {
                memcpy(this->rlock, src.rlock, sizeof (uint32_t)*16);
                this->usemap = src.usemap;
            }
            return *this;
        }

    };

}

#endif /* CONTEXT_H */

