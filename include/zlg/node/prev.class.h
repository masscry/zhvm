#pragma once
#ifndef __PREV_CLASS_HEADER__
#define __PREV_CLASS_HEADER__

namespace zlg {

    class zprev : public node {
    public:

        void prepare_node(regmap_t* map) {
            this->setErshov(1);
            map->AddRef(zhvm::RA);
            this->setResult(zhvm::RA);
        }

        void produce_node(std::ostream& output, regmap_t* map, int verbose /* = 0 */) const {
            if (verbose > 0) {
                output << "# PREV" << std::endl;
            }
            char buffer[64];
            snprintf(buffer, 64, "%s", "__prev__");
            if (this->result() < 0) {
                uint32_t freg = map->GetRegPrev();
                this->setResult(freg);
                if ((map->RefCount(this->result()) <= 1)&&(!map->CheckRegister(buffer, this->result()))) {
                    output << zhvm::GetRegisterName(this->result()) << " = add[" << zhvm::GetRegisterName(zhvm::RA) << "]" << std::endl;
                    map->MarkRegister(buffer, this->result());
                }
            } else {
                map->AddRef(zhvm::RA);
            }
            if (verbose > 0) {
                output << "# END PREV" << std::endl;
            }
        }

        zprev() : node() {
            ;
        }

        zprev(const zprev& src) : node(src) {
            ;
        }

        ~zprev() {
            ;
        }

        zprev& operator=(const zprev& src) {
            if (this != &src) {
                node::operator=(src);
            }
            return *this;
        }

    };

}

#endif // __PREV_CLASS_HEADER__