#pragma once
#ifndef __CONST_CLASS_HEADER__
#define __CONST_CLASS_HEADER__

namespace zlg {

    class zconst : public node {
        int64_t value;
    public:

        void prepare_node(regmap_t* map) {
            this->setErshov(1);
        }

        void produce_node(std::ostream& output, regmap_t* map, int verbose /* = 0 */) const {

            char buffer[64];
            snprintf(buffer, 64, "%lld", this->value);

            if (verbose > 0) {
                output << "# CONST" << this->value << std::endl;
            }

            if (this->result() < 0) {
                uint32_t freg = map->GetRegConst(buffer);
                this->setResult(freg);
            }
            if (!map->CheckRegister(buffer, this->result())) {
                // TODO: Add test on value length (long values must be loaded through memory)
                output << zhvm::GetRegisterName(this->result()) << " = add[," << buffer << "]" << std::endl;
                map->MarkRegister(buffer, this->result());
            }
            if (verbose > 0) {
                output << "# END CONST" << this->value << std::endl;
            }
        }

        zconst() : node(), value(0) {
            ;
        }

        zconst(int64_t value) : node(), value(value) {
            ;
        }

        zconst(const zconst& src) : node(src), value(src.value) {
            ;
        }

        ~zconst() {
            ;
        }

        zconst& operator=(const zconst& src) {
            if (this != &src) {
                node::operator=(src);
                this->value = src.value;
            }
            return *this;
        }

    };

}

#endif // __CONSST_CLASS_HEADER__