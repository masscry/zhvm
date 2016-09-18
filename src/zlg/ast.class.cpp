/** 
 * @file ast.class.cpp
 * @author marko
 * @date 18.09.2016
 */

#include "ast.class.h"
#include <zhvm.h>

namespace zlg {

    typedef uint32_t regmap_t;

    inline bool used(regmap_t map, uint32_t reg) {
        return ((map >> reg) & 1) != 0;
    }

    inline regmap_t setused(regmap_t map, uint32_t reg) {
        return (map | (1 << reg));
    }

    inline regmap_t resetused(regmap_t map, uint32_t reg) {
        return (map & (~(1 << reg)));
    }

    inline const char* freereg(regmap_t* map) {
        assert(map);
        for (uint32_t i = zhvm::RB; i <= zhvm::R8; ++i) {
            if (!used(*map, i)) {
                *map = setused(*map, i);
                return zhvm::GetRegisterName(i);
            }
        }
        throw std::runtime_error("NO REGISTERS LEFT");
    }

    uint32_t node::level() {
        return this->lvl;
    }

    void node::inc() {
        ++this->lvl;
    }

    node::node(uint32_t level) : lvl(level) {
        ;
    }

    node::node(const node& src) : lvl(src.lvl) {
        ;
    }

    node::~node() {
        ;
    }

    node& node::operator=(const node& src) {
        if (this != &src) {
            this->lvl = src.lvl;
        }
        return *this;
    }

    void zconst::print(FILE* output) const {
        fprintf(output, "%lld", this->value);
    }

    void zconst::produce(FILE* output, regmap_t* map) {
        this->rstr = "$A";
        if (this->level() != 0) {
            this->rstr = freereg(map);
        }
        fprintf(output, "%s = add[,%lld]\n", this->rstr, this->value);
    }

    const char* zconst::result() const {
        return this->rstr;
    }

    zconst::zconst() : node(0), value(0), rstr(0) {
        ;
    }

    zconst::zconst(int64_t value) : node(0), value(value), rstr(0) {
        ;
    }

    zconst::zconst(const zconst& src) : node(src), value(src.value), rstr(src.rstr) {
        ;
    }

    zconst::~zconst() {
        ;
    }

    zconst& zconst::operator=(const zconst& src) {
        if (this != &src) {
            node::operator=(src);
            this->value = src.value;
            this->rstr = src.rstr;
        }
        return *this;
    }

    void zbinop::print(FILE* output) const {
        const char* text = "undef";
        switch (this->id) {
            case ADD:
                text = "add";
                break;
            case SUB:
                text = "sub";
                break;
            case MUL:
                text = "mul";
                break;
            case DIV:
                text = "div";
                break;
            default:
                throw std::runtime_error("Invalid opid");
        }
        right->print(output);
        fprintf(output, " ");
        left->print(output);
        fprintf(output, " %s", text);
    }

    void zbinop::produce(FILE* output, regmap_t* map) {


    }

    const char* zbinop::result() const {
        return this->rst;
    }

    void zbinop::inc() {
        node::inc();
        left->inc();
        right->inc();
    }

    zbinop::zbinop(opid id, std::shared_ptr<node> left, std::shared_ptr<node> right) : node(0), id(id), left(left), right(right), rst(0) {
        ;
    }

    zbinop::zbinop(const zbinop& src) : node(src), id(src.id), left(src.left), right(src.right), rst(src.rst) {
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
            this->rst = src.rst;
        }
        return *this;
    }

    void ast::Print() const {
        for (std::list<std::shared_ptr<node> >::const_iterator i = items.begin(), e = items.end(); i != e; ++i) {
            (*i)->print(stdout);
            fprintf(stdout, "\n");
        }
    }

    void ast::AddItem(std::shared_ptr<node> item) {
        this->items.push_back(item);
    }

    const std::list<std::shared_ptr<node> >& ast::Items() {
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

}