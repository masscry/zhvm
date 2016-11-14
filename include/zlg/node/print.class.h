#pragma once
#ifndef __PRINT_CLASS_HEADER__
#define __PRINT_CLASS_HEADER__

namespace zlg {

    class zprint : public node {
        node_p item;
    public:

        void prepare_node(regmap_t* map) {
            this->item->prepare_node(map);
            this->setErshov(this->item->Ershov());
        }

        void produce_node(std::ostream& output, regmap_t* map, int verbose /* = 0 */) const {
            if (verbose > 0) {
                output << "# PRINT" << std::endl;
            }
            this->item->produce_node(output, map, verbose);
            if (this->item->result() != zhvm::RA) {
                output << "$A = add[" << zhvm::GetRegisterName(this->item->result()) << "]" << std::endl;
            }
            output << "cll[,0]" << std::endl;
            map->Release(this->item->result());
            if (verbose > 0) {
                output << "# END PRINT" << std::endl;
            }
        }

        zprint(node_p item) : node(), item(item) {
        }

        zprint(const zprint& src) : node(src), item(src.item) {
            ;
        }

        ~zprint() {
            ;
        }

        zprint& operator=(const zprint& src) {
            if (this != &src) {
                node::operator=(src);
                this->item = src.item;
            }
            return *this;
        }

    };

}

#endif // __PRINT_CLASS_HEADER__