#pragma once
#ifndef __ERROR_CLASS_HEADER__
#define __ERROR_CLASS_HEADER__

namespace zlg {

    class zerror : public node {
    public:

        void prepare_node(regmap_t* map) {
            this->setErshov(1);
        }

        void produce_node(std::ostream& output, regmap_t* map, int verbose) const {
            output << "# =Syntax error=" << std::endl;
            output << "nop[]" << std::endl;
        }

        zerror() {
            ;
        }

        zerror(const zerror& src) {
            ;
        }

        ~zerror() {
            ;
        }

        zerror& operator=(const zerror& src) {
            return *this;
        }

    };

}

#endif // __ERROR_CLASS_HEADER__