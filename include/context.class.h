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
#include <vector>
#include <queue>
#include <iostream>

namespace zlg {

    class context {
    public:

        typedef std::vector<std::string> reg2id_t;
        typedef std::queue<uint32_t> queue_t;
        typedef std::unordered_map<std::string, bool> vars_t;

    private:

        uint32_t refcount[16]; ///< Register allocations count
        reg2id_t regstat; ///< Register content
        queue_t queue; ///< Registers queue
        vars_t vars; ///< Variables

    public:

        context& AddRef(uint32_t reg) {
            ++this->refcount[reg];
            return *this;
        }

        context& Release(uint32_t reg) {
            --this->refcount[reg];
            if (this->refcount[reg] == 0) {
                this->queue.push(reg);
            }
            return *this;
        }

        uint32_t RefCount(uint32_t reg) {
            return this->refcount[reg];
        }

        int32_t SearchID(const char* id) {
            for (int32_t i = 0; i < 16; ++i) {
                if (regstat[i].compare(id) == 0) {
                    return i;
                }
            }
            return -1;
        }

        void RegisterVariable(const std::string& var) {
            if (vars.find(var) == vars.end()) {
                vars[var] = false;
            }
        }

        bool IsVariableProduced(const std::string& var) {
            vars_t::iterator item = vars.find(var);
            if (item != vars.end()) {
                return item->second;
            }
            throw std::runtime_error("Variable not found");
        }

        void ProduceVariable(std::ostream& output, const std::string& var) {
            output << "!data" << std::endl;
            vars_t::iterator item = vars.find(var);
            if ((item != vars.end()) && (!item->second)) {
                output << "!" << item->first << std::endl;
                output << "!0q" << std::endl;
                item->second = true;
            }
            output << "!code" << std::endl;
        }

        void ProduceVariables(std::ostream& output) {
            output << "!data" << std::endl;
            for (vars_t::iterator i = vars.begin(), e = vars.end(); i != e; ++i) {
                if (!i->second) {
                    output << "!" << i->first << std::endl;
                    output << "!0q" << std::endl;
                    i->second = true;
                }
            }
            output << "!code" << std::endl;
        }

        /**
         * Search register for constant
         * 
         * @param id - constant id
         * @return register or throw exception
         */
        uint32_t GetRegConst(const char* id) {
            int32_t rid = SearchID(id); // Search item in id map
            if (rid != -1) {
                // If item found                
                if (regstat[rid].compare(id) == 0) { // And register content is still there
                    this->AddRef(rid); // Do not let to re-assign this register
                    return rid;
                }
            }

            // On first pass traverse through registers and find which are not used at all.
            for (uint32_t i = 0; i < 16; ++i) {
                uint32_t result = this->queue.front();
                this->queue.pop();
                if ((this->refcount[result] == 0)&&(this->regstat[result].empty())) {
                    this->AddRef(result);
                    return result;
                }
                this->queue.push(result);
            }

            // On second pass traverse through registers and find which are used, but has some data
            for (uint32_t i = 0; i < 16; ++i) {
                uint32_t result = this->queue.front();
                this->queue.pop();
                if (this->refcount[result] == 0) { // Replace data
                    this->AddRef(result);
                    return result;
                }
                this->queue.push(result);
            }
            throw std::runtime_error("No registers left");
        }

        void MarkRegister(const char* id, uint32_t reg) {
            this->regstat[reg] = id;
        }

        bool CheckRegister(const char* id, uint32_t reg) {
            return (this->regstat[reg].compare(id) == 0);
        }

        void ClearRegister(uint32_t reg) {
            this->regstat[reg].clear();
        }

        void Reset() {
            while (!this->queue.empty()) {
                this->queue.pop();
            }
            for (int i = 0; i < 16; ++i) {
                refcount[i] = 0;
                regstat[i].clear();
                this->queue.push(i);
            }
        }

        uint32_t CountFreeRegisters() {
            uint32_t result = 0;
            for (uint32_t i = 0; i < 16; ++i) {
                result += (this->RefCount(i) == 0);
            }
            return result;
        }

        /**
         * Get register for binary operation
         * 
         * @return register or throw exception
         */
        uint32_t GetRegBinOp() {
            // On first pass traverse through registers and find which are not used at all.
            for (uint32_t i = 0; i < 16; ++i) {
                uint32_t result = this->queue.front();
                this->queue.pop();
                if ((this->refcount[result] == 0)&&(this->regstat[result].empty())) {
                    this->ClearRegister(result); // Hold temporary data, so can be used as soon as refcount == 0
                    this->AddRef(result);
                    return result;
                }
                this->queue.push(result);
            }

            // On second pass traverse through registers and find which are used, but has some data
            for (uint32_t i = 0; i < 16; ++i) {
                uint32_t result = this->queue.front();
                this->queue.pop();
                if (this->refcount[result] == 0) { // Replace data
                    this->ClearRegister(result); // Hold temporary data, so can be used as soon as refcount == 0
                    this->AddRef(result);
                    return result;
                }
                this->queue.push(result);
            }
            throw std::runtime_error("No registers left");
        }

        uint32_t GetRegPrev() {
            return GetRegConst("__prev__");
        }

        context() : refcount(), regstat() {
            this->regstat.resize(16, "");
            for (uint32_t i = 0; i < 16; ++i) {
                this->refcount[i] = 0;
                this->queue.push(i);
            }
        }

        context(const context& copy) : refcount(), regstat(copy.regstat) {
            memcpy(refcount, copy.refcount, sizeof (uint32_t)*16);
        }

        ~context() {
            ;
        }

    };



}

#endif /* CONTEXT_H */

