/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   context.cpp
 * Author: timur
 * 
 * Created on 26 сентября 2016 г., 17:16
 */

#include "context.class.h"
#include <zhvm.h>
#include <bits/string3.h>

namespace zlg {

    void context::MarkRegister(int usage, const char* id, uint32_t reg) {
        switch (usage) {
            case RU_CONSTANT:
            case RU_PREV_VAL:
            {
                this->usemap[id] = reg;
                break;
            }
            case RU_VALUE:
                break;
        }
    }

    uint32_t context::GetFreeRegister(int usage, const char* id) {

        switch (usage) {
            case RU_CONSTANT:
            case RU_PREV_VAL:
            {
                usemap_t::iterator i = this->usemap.find(id);
                if (i != this->usemap.end()) {
                    if (this->RefCount(i->second) >= 0) {
                        this->AddRef(i->second);
                        return i->second;
                    }
                    this->usemap.erase(id);
                }
                break;
            }
            case RU_VALUE:
                break;
            default:
                throw std::runtime_error("Unknown usage");
        }
        for (uint32_t i = zhvm::RZ; i < zhvm::RTOTAL; ++i) {
            if (this->RefCount(i) == 0) {
                this->AddRef(i);
                return i;
            }
        }
        throw std::runtime_error("No free registers left");
    }

    bool context::CheckRegister(const char* id, uint32_t reg) {
        usemap_t::iterator i = this->usemap.find(id);
        return ((i != this->usemap.end())&&(i->second == reg));
    }

    uint32_t context::CountFreeRegsiters() const {
        uint32_t result = 0;
        for (uint32_t i = zhvm::RZ; i < zhvm::RTOTAL; ++i) {
            result += (this->RefCount(i) == 0);
        }
        return result;
    }

    uint32_t context::RefCount(uint32_t reg) const {
        if (reg >= 16) {
            throw std::runtime_error("Register out of range");
        }
        return rlock[reg];
    }

    void context::ClearMap(uint32_t reg) {
        for (usemap_t::iterator i = this->usemap.begin(), e = this->usemap.end(); i != e;) {
            if (i->second == reg) {
                i = this->usemap.erase(i);
            } else {
                ++i;
            }
        }
    }

    void context::TotalReset() {
        this->usemap.clear();
        memset(this->rlock, 0, sizeof (uint32_t)*16);
    }

    void context::AddRef(uint32_t reg) {
        if (reg >= 16) {
            throw std::runtime_error("Register out of range");
        }
        ++rlock[reg];
    }

    void context::Release(uint32_t reg) {
        if (reg >= 16) {
            throw std::runtime_error("Register out of range");
        }
        --rlock[reg];
    }

}
