#pragma once
#ifndef __WHILE_CLASS_HEADER__
#define __WHILE_CLASS_HEADER__

namespace zlg {

    class zwhile : public node {
        node_p cond;
        node_p trueb;

        uint32_t uid;

    public:

        void prepare_node(regmap_t* map) {
            this->cond->prepare_node(map);
            this->trueb->prepare_node(map);
            this->setErshov(std::max(this->cond->Ershov(), this->trueb->Ershov()));
            this->uid = map->Counter();
        }

        void produce_node(std::ostream& output, regmap_t* map, int verbose) const {
            output << "!__while_start__" << this->uid << std::endl;
            this->cond->produce_node(output, map, verbose);
            output << zhvm::GetRegisterName(zhvm::RP) << " = cmz[" << zhvm::GetRegisterName(this->cond->result()) << ", @__while_end__" << this->uid << "]" << std::endl;
            map->Release(this->cond->result());
            this->trueb->produce_node(output, map, verbose);
            output << zhvm::GetRegisterName(zhvm::RP) << " = add[,@__while_start__" << this->uid << "]" << std::endl;
            output << "!__while_end__" << this->uid << std::endl;
            output << "nop[]" << std::endl;
            this->setResult(this->trueb->result());
        }

        zwhile(node_p cond, node_p trueb) : cond(cond), trueb(trueb), uid(0) {

        }

        zwhile(const zwhile& src) : cond(src.cond), trueb(src.trueb), uid(src.uid) {

        }

        ~zwhile() {
            ;
        }

        zwhile& operator=(const zwhile& src) {
            if (this != &src) {
                this->cond = src.cond;
                this->trueb = src.trueb;
                this->uid = src.uid;
            }
            return *this;
        }

    };

}

#endif // __WHILE_CLASS_HEADER__