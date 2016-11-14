#pragma once
#ifndef __IFELSE_CLASS_HEADER__
#define __IFELSE_CLASS_HEADER__

namespace zlg {

    class zifelse : public node {
        node_p cond;
        node_p trueb;
        node_p falseb;

        uint32_t uid;

    public:

        void prepare_node(regmap_t* map) {
            this->cond->prepare_node(map);
            this->trueb->prepare_node(map);
            this->falseb->prepare_node(map);
            this->setErshov(std::max(std::max(this->cond->Ershov(), this->trueb->Ershov()), this->falseb->Ershov()));
            this->uid = map->Counter();
        }

        void produce_node(std::ostream& output, regmap_t* map, int verbose) const {
            this->cond->produce_node(output, map, verbose);
            output << zhvm::GetRegisterName(zhvm::RP) << " = cmz[" << zhvm::GetRegisterName(this->cond->result()) << ", @__if__" << this->uid << "]" << std::endl;
            map->Release(this->cond->result());
            this->trueb->produce_node(output, map, verbose);
            output << zhvm::GetRegisterName(zhvm::RP) << " = add[, @__else__" << this->uid << "]" << std::endl;
            output << "!__if__" << this->uid << std::endl;
            this->falseb->produce_node(output, map, verbose);
            output << "!__else__" << this->uid << std::endl;
            output << "nop[]" << std::endl;
            this->setResult(-1);
        }

        zifelse(node_p cond, node_p trueb, node_p falseb) : cond(cond), trueb(trueb), falseb(falseb), uid(0) {

        }

        zifelse(const zifelse& src) : cond(src.cond), trueb(src.trueb), falseb(src.falseb), uid(src.uid) {

        }

        ~zifelse() {
            ;
        }

        zifelse& operator=(const zifelse& src) {
            if (this != &src) {
                this->cond = src.cond;
                this->trueb = src.trueb;
                this->falseb = src.falseb;
                this->uid = src.uid;
            }
            return *this;
        }

    };


}

#endif // __IF_CLASS_HEADER__