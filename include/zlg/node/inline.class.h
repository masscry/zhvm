#pragma once
#ifndef __INLINE_CLASS_HEADER__
#define __INLINE_CLASS_HEADER__

namespace zlg {

    class zinline : public node {
        std::string text;

    public:

        void prepare_node(regmap_t* map) {
            this->setErshov(1);
        }

        void produce_node(std::ostream& output, regmap_t* map, int verbose /* = 0 */) const {
            if (verbose > 0) {
                output << "# INLINE" << std::endl;
            }
            output << this->text << std::endl;

            if (verbose > 0) {
                output << "# END INLINE" << std::endl;
            }
        }

        zinline() : node(), text() {
            ;
        }

        zinline(const std::string& text) : node(), text(text) {
            ;
        }

        zinline(const char* text) : node(), text(text) {
            ;
        }

        zinline(const zinline& src) : node(src), text(src.text) {
            ;
        }

        ~zinline() {
            ;
        }

        zinline& operator=(const zinline& src) {
            if (this != &src) {
                node::operator=(src);
                this->text = src.text;
            }
            return *this;
        }

    };





}

#endif // __INLINE_CLASS_HEADER__