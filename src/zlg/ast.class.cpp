/** 
 * @file ast.class.cpp
 * @author marko
 * @date 18.09.2016
 */

#include "ast.class.h"
#include <zhvm.h>
#include <zlgy.gen.hpp>
#include <zlg.gen.h>

namespace zlg {

    typedef uint32_t regmap_t;

    inline bool used(regmap_t map, uint32_t reg) {
        assert(reg < zhvm::RTOTAL);
        return ((map >> reg) & 1) != 0;
    }

    inline regmap_t setused(regmap_t map, uint32_t reg) {
        assert(reg < zhvm::RTOTAL);
        return (map | (1 << reg));
    }

    inline regmap_t resetused(regmap_t map, uint32_t reg) {
        assert(reg < zhvm::RTOTAL);
        return (map & (~(1 << reg)));
    }

    inline uint32_t freereg(regmap_t* map) {
        assert(map);
        for (uint32_t i = zhvm::RA; i <= zhvm::R8; ++i) {
            if (!used(*map, i)) {
                *map = setused(*map, i);
                return i;
            }
        }
        throw std::runtime_error("NO REGISTERS LEFT");
    }

    inline uint32_t usedregcount(regmap_t map) {
        uint32_t result = 0;
        for (uint32_t i = zhvm::RA; i <= zhvm::R8; ++i) {
            if (!used(map, i)) {
                ++result;
            }
        }
        return result;
    }

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

    void node::Produce(std::ostream& output) const {
        zlg::regmap_t map = 0;
        this->setResult(zhvm::RA);
        map = setused(map, zhvm::RA);
        this->produce_node(output, &map);
    }

    void zconst::prepare_node() {
        this->setErshov(1);
    }

    void zconst::produce_node(std::ostream& output, regmap_t* map) const {
        if (this->result() < 0) {
            this->setResult(freereg(map));
        }
        output << zhvm::GetRegisterName(this->result()) << " = add[," << this->value << "]" << std::endl;
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

    void zbinop::prepare_node() {
        this->right->prepare_node();
        this->left->prepare_node();

        this->setErshov(// if c1 == c2 then c1+1, else max(c1, c2)
                (this->right->Ershov() == this->left->Ershov()) ?
                (this->right->Ershov() + 1) :
                (std::max(this->right->Ershov(), this->left->Ershov()))
                );
    }

    void zbinop::produce_node(std::ostream& output, regmap_t* map) const {
        if (this->Ershov() > usedregcount(*map)) {
            throw std::runtime_error("Not enough reigsters");
        }
        if (this->right->Ershov() > this->left->Ershov()) {
            this->right->produce_node(output, map);
            this->left->produce_node(output, map);
            if (this->result() < 0) {
                this->setResult(this->right->result());
            }
            *map = resetused(*map, this->left->result());
        } else {
            this->left->produce_node(output, map);
            this->right->produce_node(output, map);
            if (this->result() < 0) {
                this->setResult(this->left->result());
            }
            *map = resetused(*map, this->right->result());
        }

        const char* optext = "undef";
        switch (this->id) {
            case ADD:
                optext = "add";
                break;
            case SUB:
                optext = "sub";
                break;
            case MUL:
                optext = "mul";
                break;
            case DIV:
                optext = "div";
                break;
            default:
                throw std::runtime_error("Invalid opid");
        }
        output << zhvm::GetRegisterName(this->result()) << " = " << optext << "[" << zhvm::GetRegisterName(this->left->result()) << "," << zhvm::GetRegisterName(this->right->result()) << "]" << std::endl;
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

    void zinline::prepare_node() {
        this->setErshov(1);
    }

    void zinline::produce_node(std::ostream& output, regmap_t* map) const {
        output << this->text << std::endl;
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

    void zprint::prepare_node() {
        this->item->prepare_node();
        this->setErshov(this->item->Ershov());
    }

    void zprint::produce_node(std::ostream& output, regmap_t* map) const {
        this->item->produce_node(output, map);
        if (this->item->result() != zhvm::RA) {
            output << "$A = add[" << zhvm::GetRegisterName(this->item->result()) << "]" << std::endl;
        }
        output << "cll[,0]" << std::endl;
        *map = resetused(*map, this->item->result());
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

    void zprev::prepare_node() {
        this->setErshov(1);
    }

    void zprev::produce_node(std::ostream& output, regmap_t* map) const {
        if (this->result() < 0) {
            this->setResult(freereg(map));
        }
        output << zhvm::GetRegisterName(this->result()) << " = add[" << zhvm::GetRegisterName(zhvm::RA) << "]" << std::endl;
    }

    zprev::zprev() : node() {
        ;
    }

    zprev::zprev(const zprint& src) : node(src) {
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

    void ast::Prepare() {
        for (std::list<std::shared_ptr<zlg::node> >::const_iterator i = this->Items().begin(), e = this->Items().end(); i != e; ++i) {
            (*i)->prepare_node();
        }
    }

    void ast::Produce(std::ostream& output) const {
        for (std::list<std::shared_ptr<zlg::node> >::const_iterator i = this->Items().begin(), e = this->Items().end(); i != e; ++i) {
            (*i)->Produce(output);
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