#pragma once
#ifndef __VAR_CLASS_HEADER__
#define __VAR_CLASS_HEADER__

namespace zlg {

    class zvar : public node {
        std::string id;

    public:

        void prepare_node(regmap_t* map) {
            map->RegisterVariable(this->id);
            this->setErshov(1);
        }

        void produce_node(std::ostream& output, regmap_t* map, int verbose /* = 0 */) const {
            if (verbose > 0) {
                output << "# VAR" << this->id << std::endl;
            }
            if (!map->IsVariableProduced(this->id)) {
                std::cerr << "Undefined variable: " << this->id << std::endl;
                throw std::runtime_error("Undefined variable");
            }
            char buffer[64];
            snprintf(buffer, 64, "@%s", this->id.c_str());
            if (this->result() < 0) {
                uint32_t freg = map->GetRegConst(buffer);
                this->setResult(freg);
            }
            if (!map->CheckRegister(buffer, this->result())) {
                // TODO: Add test on value length (long values must be loaded through memory)
                output << zhvm::GetRegisterName(this->result()) << " = add[," << buffer << "]" << std::endl;
                output << zhvm::GetRegisterName(this->result()) << " = ldq[" << zhvm::GetRegisterName(this->result()) << "]" << std::endl;
                map->MarkRegister(buffer, this->result());
            }
            if (verbose > 0) {
                output << "# END VAR" << std::endl;
            }
        }

        zvar(const char* id) : id() {
            char buffer[128];
            snprintf(buffer, 128, "_var_%s_", id);
            this->id = buffer;
        }

        zvar(const zvar& src) : id(src.id) {

        }

        ~zvar() {

        }

        zvar& operator=(const zvar& src) {
            if (this != &src) {
                this->id = src.id;
            }
            return *this;
        }

        const std::string& VarID() const {
            return this->id;
        }

    };

}

#endif // __VAR_CLASS_HEADER__