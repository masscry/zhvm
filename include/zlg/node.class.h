#pragma once
#ifndef __NODE_CLASS_HEADER__
#define __NODE_CLASS_HEADER__

#include <memory>

#include <zhvm.h>

namespace zlg {

    class node {
        uint32_t ershov;
        mutable int rstr;

    public:

        virtual void prepare_node(regmap_t* map) = 0;
        virtual void produce_node(std::ostream& output, regmap_t* map, int verbose) const = 0;

        virtual int result() const {
            return this->rstr;
        }

        virtual void setResult(int nr) const {
            this->rstr = nr;
        }

        void Produce(std::ostream& output, regmap_t* map, int verbose) const {

            map->AddRef(zhvm::RZ); // Zero used implicitly
            map->AddRef(zhvm::RD); // RESERVED
            map->AddRef(zhvm::RS); // RESERVED
            map->AddRef(zhvm::RP); // ALWAYS RESERVED

            this->setResult(zhvm::RA);
            this->produce_node(output, map, verbose);

        }

        node() : ershov(0), rstr(-1) {
            ;
        }

        node(const node& src) : ershov(src.ershov), rstr(src.rstr) {
            ;
        }

        virtual ~node() {
            ;
        }

        node& operator=(const node& src) {
            if (this != &src) {
                this->ershov = src.ershov;
                this->rstr = src.rstr;
            }
            return *this;
        }

        void setErshov(uint32_t ershov) {
            this->ershov = ershov;
        }

        uint32_t Ershov() const {
            return ershov;
        }
    };

    typedef std::shared_ptr<node> node_p;

}

#include "node/binop.class.h"
#include "node/block.class.h"
#include "node/const.class.h"
#include "node/error.class.h"
#include "node/if.class.h"
#include "node/ifelse.class.h"
#include "node/inline.class.h"
#include "node/null.class.h"
#include "node/prev.class.h"
#include "node/print.class.h"
#include "node/unop.class.h"
#include "node/var.class.h"
#include "node/while.class.h"

#endif // __NODE_CLASS_HEADER__