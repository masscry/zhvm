#pragma once
#ifndef __BLOCK_CLASS_HEADER__
#define __BLOCK_CLASS_HEADER__

namespace zlg {

    class zblock : public node {
        std::list<node_p> items;

    public:

        void add_item(node_p item) {
            this->items.push_back(item);
        }

        void prepare_node(regmap_t* map) {
            for (std::list<node_p>::iterator i = this->items.begin(), e = this->items.end(); i != e; ++i) {
                (*i)->prepare_node(map);
            }
        }

        void produce_node(std::ostream& output, regmap_t* map, int verbose) const {
            if (verbose > 0) {
                output << "# BLOCK" << std::endl;
            }
            if (!this->items.empty()) {
                for (std::list<node_p>::const_iterator i = this->items.begin(), e = this->items.end(); i != e; ++i) {
                    (*i)->produce_node(output, map, verbose);
                }
            } else {
                output << "nop[]" << std::endl;
            }
            if (verbose > 0) {
                output << "# END BLOCK" << std::endl;
            }
        }

        zblock() : items() {

        }

        zblock(const zblock& src) : items(src.items) {

        }

        ~zblock() {

        }

        zblock& operator=(const zblock& src) {
            if (this != &src) {
                this->items = src.items;
            }
            return *this;
        }

    };

}

#endif // __BLOCK_CLASS_HEADER__