#pragma once
#ifndef __BINOP_CLASS_HEADER__
#define __BINOP_CLASS_HEADER__

#include "var.class.h"

namespace zlg {

    class zbinop : public node {
    public:

        enum opid {
            UNDEF,
            ADD,
            SUB,
            MUL,
            DIV,
            SET,
            AND,
            OR,
            GR,
            LS,
            GRE,
            LSE,
            EQ,
            NEQ,
            COMMA
        };

    private:

        opid id;
        node_p left;
        node_p right;

        static const char* OPIDString(opid id) {
            switch (id) {
                case UNDEF:
                    return "undef";
                case ADD:
                    return "add";
                case SUB:
                    return "sub";
                case MUL:
                    return "mul";
                case DIV:
                    return "div";
                case SET:
                    return "set";
                case AND:
                    return "and";
                case OR:
                    return "or";
                case GR:
                    return "gr";
                case LS:
                    return "ls";
                case GRE:
                    return "gre";
                case LSE:
                    return "lse";
                default:
                    return "???";
            }
        }

    public:

        void prepare_node(regmap_t* map) {


            this->right->prepare_node(map);
            this->left->prepare_node(map);

            this->setErshov(// if c1 == c2 then c1+1, else max(c1, c2)
                    (this->right->Ershov() == this->left->Ershov()) ?
                    (this->right->Ershov() + 1) :
                    (std::max(this->right->Ershov(), this->left->Ershov()))
                    );
        }

        void produce_node(std::ostream& output, regmap_t* map, int verbose /* = 0 */) const {

            if (this->id != SET) {
                if (verbose > 0) {
                    output << "# BINOP" << OPIDString(this->id) << std::endl;
                }

                if (this->Ershov() > map->CountFreeRegisters()) {
                    throw std::runtime_error("Not enough reigsters");
                }

                if (this->right->Ershov() > this->left->Ershov()) {
                    this->right->produce_node(output, map, verbose);
                    this->left->produce_node(output, map, verbose);

                    map->Release(this->right->result());
                    if (this->result() < 0) {
                        this->setResult(map->GetRegBinOp());
                    }
                    map->Release(this->left->result());
                } else {
                    this->left->produce_node(output, map, verbose);
                    this->right->produce_node(output, map, verbose);
                    map->Release(this->left->result());
                    if (this->result() < 0) {
                        this->setResult(map->GetRegBinOp());
                    }
                    map->Release(this->right->result());
                }

                const char* optext = 0;
                switch (this->id) {
                    case ADD:
                    case SUB:
                    case MUL:
                    case DIV:
                    case AND:
                    case OR:
                    case GR:
                    case LS:
                    case GRE:
                    case LSE:
                        optext = OPIDString(this->id);
                        break;
                    default:
                        throw std::runtime_error("Invalid opid");
                }
                output << zhvm::GetRegisterName(this->result()) << " = " << optext << "[" << zhvm::GetRegisterName(this->left->result()) << "," << zhvm::GetRegisterName(this->right->result()) << "]" << std::endl;

            } else {

                zvar* leftitem = dynamic_cast<zvar*> (this->left.get());
                if (leftitem == 0) {
                    throw std::runtime_error("Can't set value to expression");
                }

                this->right->produce_node(output, map, verbose);
                map->Release(this->right->result());

                if (this->result() < 0) {
                    this->setResult(map->GetRegBinOp());
                    map->AddRef(this->result());
                }

                map->ProduceVariable(output, leftitem->VarID());

                char buffer[64];
                snprintf(buffer, 64, "@%s", leftitem->VarID().c_str());
                if (leftitem->result() < 0) {
                    uint32_t freg = map->GetRegConst(buffer);
                    leftitem->setResult(freg);
                }

                if (verbose > 0) {
                    output << "# BINOP" << OPIDString(this->id) << ": " << buffer << std::endl;
                }
                output << zhvm::GetRegisterName(leftitem->result()) << " = add[," << buffer << "]" << std::endl;
                output << zhvm::GetRegisterName(leftitem->result()) << " = svq[" << zhvm::GetRegisterName(this->right->result()) << "]" << std::endl;
                output << zhvm::GetRegisterName(leftitem->result()) << " = add[" << zhvm::GetRegisterName(this->right->result()) << "]" << std::endl; // HACK
            }

            if (verbose > 0) {
                output << "# END BINOP" << std::endl;
            }
        }

        zbinop(opid id, node_p left, node_p right) : node(), id(id), left(left), right(right) {
        }

        zbinop(const zbinop& src) : node(src), id(src.id), left(src.left), right(src.right) {
            ;
        }

        ~zbinop() {
            ;
        }

        zbinop& operator=(const zbinop& src) {
            if (this != &src) {
                node::operator=(src);
                this->id = src.id;
                this->left = src.left;
                this->right = src.right;
            }
            return *this;
        }

    };





}

#endif // __BINOP_CLASS_HEADER__