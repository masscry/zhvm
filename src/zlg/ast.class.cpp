/** 
 * @file ast.class.cpp
 * @author marko
 * @date 18.09.2016
 */

#include "ast.class.h"
#include <zhvm.h>
#include <zlgy.gen.hpp>
#include <zlg.gen.h>
#include <bits/stdio2.h>

namespace zlg {

    node::node() : ershov(0), rstr(-1) {
        ;
    }

    node::node(const node& src) : ershov(src.ershov), rstr(src.rstr) {
        ;
    }

    node::~node() {
        ;
    }

    node& node::operator=(const node& src) {
        if (this != &src) {
            this->ershov = src.ershov;
            this->rstr = src.rstr;
        }
        return *this;
    }

    void node::setErshov(uint32_t ershov) {
        this->ershov = ershov;
    }

    uint32_t node::Ershov() const {
        return ershov;
    }

    int node::result() const {
        return this->rstr;
    }

    void node::setResult(int nr) const {
        this->rstr = nr;
    }

    void node::Produce(std::ostream& output, regmap_t* map, int verbose) const {

        map->AddRef(zhvm::RZ); // Zero used implicitly
        map->AddRef(zhvm::RD); // RESERVED
        map->AddRef(zhvm::RS); // RESERVED
        map->AddRef(zhvm::RP); // ALWAYS RESERVED

        this->setResult(zhvm::RA);
        this->produce_node(output, map, verbose);
    }

    void zconst::prepare_node(regmap_t* map) {
        this->setErshov(1);
    }

    void zconst::produce_node(std::ostream& output, regmap_t* map, int verbose /* = 0 */) const {

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

    zconst::zconst() : node(), value(0) {
        ;
    }

    zconst::zconst(int64_t value) : node(), value(value) {
        ;
    }

    zconst::zconst(const zconst& src) : node(src), value(src.value) {
        ;
    }

    zconst::~zconst() {
        ;
    }

    zconst& zconst::operator=(const zconst& src) {
        if (this != &src) {
            node::operator=(src);
            this->value = src.value;
        }
        return *this;
    }

    void zbinop::prepare_node(regmap_t* map) {


        this->right->prepare_node(map);
        this->left->prepare_node(map);

        this->setErshov(// if c1 == c2 then c1+1, else max(c1, c2)
                (this->right->Ershov() == this->left->Ershov()) ?
                (this->right->Ershov() + 1) :
                (std::max(this->right->Ershov(), this->left->Ershov()))
                );
    }

    const char* zbinop::OPIDString(opid id) {
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

    void zbinop::produce_node(std::ostream& output, regmap_t* map, int verbose /* = 0 */) const {

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

        }

        if (verbose > 0) {
            output << "# END BINOP" << std::endl;
        }
    }

    zbinop::zbinop(opid id, std::shared_ptr<node> left, std::shared_ptr<node> right) : node(), id(id), left(left), right(right) {
    }

    zbinop::zbinop(const zbinop& src) : node(src), id(src.id), left(src.left), right(src.right) {
        ;
    }

    zbinop::~zbinop() {
        ;
    }

    zbinop& zbinop::operator=(const zbinop& src) {
        if (this != &src) {
            node::operator=(src);
            this->id = src.id;
            this->left = src.left;
            this->right = src.right;
        }
        return *this;
    }

    const char* zunop::OPIDString(opid id) {
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

    void zunop::prepare_node(regmap_t* map) {
        this->right->prepare_node(map);
        this->setErshov(this->right->Ershov());
    }

    void zunop::produce_node(std::ostream& output, regmap_t* map, int verbose) const {
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

    zunop::zunop(opid id, std::shared_ptr<node> right) : id(id), right(right) {
        ;
    }

    zunop::zunop(const zunop& src) : id(src.id), right(src.right) {
        ;
    }

    zunop::~zunop() {
        ;
    }

    zunop& zunop::operator=(const zunop& src) {
        if (this != &src) {
            this->id = src.id;
            this->right = src.right;
        }
        return *this;
    }

    void zinline::prepare_node(regmap_t* map) {
        this->setErshov(1);
    }

    void zinline::produce_node(std::ostream& output, regmap_t* map, int verbose /* = 0 */) const {
        if (verbose > 0) {
            output << "# INLINE" << std::endl;
        }
        output << this->text << std::endl;

        if (verbose > 0) {
            output << "# END INLINE" << std::endl;
        }
    }

    zinline::zinline() : node(), text() {
        ;
    }

    zinline::zinline(const std::string& text) : node(), text(text) {
        ;
    }

    zinline::zinline(const char* text) : node(), text(text) {
        ;
    }

    zinline::zinline(const zinline& src) : node(src), text(src.text) {
        ;
    }

    zinline::~zinline() {
        ;
    }

    zinline& zinline::operator=(const zinline& src) {
        if (this != &src) {
            node::operator=(src);
            this->text = src.text;
        }
        return *this;
    }

    void zprint::prepare_node(regmap_t* map) {
        this->item->prepare_node(map);
        this->setErshov(this->item->Ershov());
    }

    void zprint::produce_node(std::ostream& output, regmap_t* map, int verbose /* = 0 */) const {
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

    zprint::zprint(std::shared_ptr<node> item) : node(), item(item) {
    }

    zprint::zprint(const zprint& src) : node(src), item(src.item) {
        ;
    }

    zprint::~zprint() {
        ;
    }

    zprint& zprint::operator=(const zprint& src) {
        if (this != &src) {
            node::operator=(src);
            this->item = src.item;
        }
        return *this;
    }

    void zprev::prepare_node(regmap_t* map) {
        this->setErshov(1);
        map->AddRef(zhvm::RA);
        this->setResult(zhvm::RA);
    }

    void zprev::produce_node(std::ostream& output, regmap_t* map, int verbose /* = 0 */) const {
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

    zprev::zprev() : node() {
        ;
    }

    zprev::zprev(const zprev& src) : node(src) {
        ;
    }

    zprev::~zprev() {
        ;
    }

    zprev& zprev::operator=(const zprev& src) {
        if (this != &src) {
            node::operator=(src);
        }
        return *this;
    }

    void zvar::prepare_node(regmap_t* map) {
        map->RegisterVariable(this->id);
        this->setErshov(1);
    }

    void zvar::produce_node(std::ostream& output, regmap_t* map, int verbose /* = 0 */) const {
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
            output << zhvm::GetRegisterName(this->result()) << " = ldq[," << buffer << "]" << std::endl;
            map->MarkRegister(buffer, this->result());
        }
        if (verbose > 0) {
            output << "# END VAR" << std::endl;
        }
    }

    zvar::zvar(const char* id) : id() {
        char buffer[128];
        snprintf(buffer, 128, "_var_%s_", id);
        this->id = buffer;
    }

    zvar::zvar(const zvar& src) : id(src.id) {

    }

    zvar::~zvar() {

    }

    zvar& zvar::operator=(const zvar& src) {
        if (this != &src) {
            this->id = src.id;
        }
        return *this;
    }

    const std::string& zvar::VarID() const {
        return this->id;
    }

    void ast::Scan() {
        yyscan_t scan;

        zlg::ast temp;

        yylex_init(&scan);
        yyparse(scan, temp);
        yylex_destroy(scan);
        *this = std::move(temp);
    }

    void ast::Scan(const char* text) {
        yyscan_t scan;

        zlg::ast temp;

        yylex_init(&scan);
        YY_BUFFER_STATE input = yy_scan_string(text, scan);

        yyparse(scan, temp);

        yy_delete_buffer(input, scan);
        yylex_destroy(scan);
        *this = std::move(temp);
    }

    void ast::Generate(std::ostream& output, context* map, int verbose) {
        for (std::list<std::shared_ptr<zlg::node> >::const_iterator i = this->Items().begin(), e = this->Items().end(); i != e; ++i) {
            map->Reset();
            (*i)->prepare_node(map);
            (*i)->Produce(output, map, verbose);
        }
    }

    void ast::AddItem(std::shared_ptr<node> item) {
        this->items.push_back(item);
    }

    const std::list<std::shared_ptr<node> >& ast::Items() const {
        return this->items;
    }

    ast::ast() {
        ;
    }

    ast::ast(const ast& orig) : items(orig.items) {
        ;
    }

    ast::~ast() {
    }

    ast& ast::operator=(const ast& src) {
        if (this != &src) {
            this->items.assign(src.items.begin(), src.items.end());
        }
        return *this;
    }

    ast& ast::operator=(ast&& src) {
        if (this != &src) {
            this->items = std::move(src.items);
        }
        return *this;
    }

}