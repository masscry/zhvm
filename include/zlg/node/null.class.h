#pragma once
#ifndef __NULL_CLASS_HEADER__
#define __NULL_CLASS_HEADER__

namespace zlg {

    class znull : public node {
    public:

        void prepare_node(regmap_t* map) {
            ;
        }

        void produce_node(std::ostream& output, regmap_t* map, int verbose) const {
            output << "nop[]" << std::endl;
        }

        znull() {
            ;
        }

        znull(const znull& src) {
            ;
        }

        ~znull() {
            ;
        }

        znull& operator=(const znull& src) {
            return *this;
        }
    };


}

#endif // __NULL_CLASS_HEADER__