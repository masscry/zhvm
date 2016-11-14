#pragma once
#ifndef __UNOP_CLASS_HEADER__
#define __UNOP_CLASS_HEADER__

namespace zlg {

    class zunop : public node {
    public:

        enum opid {
            UNDEF,
            PLUS,
            MINUS,
            NOT
        };

    private:

        opid id;
        node_p right;

        static const char* OPIDString(opid id) {
            switch (id) {
                case UNDEF:
                    return "undef";
                case MINUS:
                    return "sub";
                case NOT:
                    return "not";
                default:
                    return "???";
            }
        }


    public:

        void prepare_node(regmap_t* map) {
            this->right->prepare_node(map);
            this->setErshov(this->right->Ershov());
        }

        void produce_node(std::ostream& output, regmap_t* map, int verbose) const {
            if (verbose > 0) {
                output << "# UNOP" << OPIDString(this->id) << std::endl;
            }

            if (this->Ershov() > map->CountFreeRegisters()) {
                throw std::runtime_error("Not enough reigsters");
            }

            this->right->produce_node(output, map, verbose);

            map->Release(this->right->result());
            if (this->result() < 0) {
                this->setResult(map->GetRegBinOp());
            }

            const char* optext = 0;
            switch (this->id) {
                case MINUS:
                case NOT:
                    optext = OPIDString(this->id);
                    break;
                default:
                    throw std::runtime_error("Invalid opid");
            }
            output << zhvm::GetRegisterName(this->result()) << " = " << optext << "[" << zhvm::GetRegisterName(zhvm::RZ) << "," << zhvm::GetRegisterName(this->right->result()) << "]" << std::endl;

            if (verbose > 0) {
                output << "# END UNOP" << std::endl;
            }
        }

        zunop(opid id, node_p right) : id(id), right(right) {
            ;
        }

        zunop(const zunop& src) : id(src.id), right(src.right) {
            ;
        }

        ~zunop() {
            ;
        }

        zunop& operator=(const zunop& src) {
            if (this != &src) {
                this->id = src.id;
                this->right = src.right;
            }
            return *this;
        }

    };






}

#endif // __UNOP_CLASS_HEADER__