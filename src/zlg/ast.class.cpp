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
        for (uint32_t i = zhvm::RB; i <= zhvm::R8; ++i) {
            if (!used(*map, i)) {
                *map = setused(*map, i);
                return i;
            }
        }
        throw std::runtime_error("NO REGISTERS LEFT");
    }

    uint32_t node::level() const {
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

    void node::Produce(std::ostream& output) const {
        zlg::regmap_t map = 0;
        this->produce_node(output, &map);
    }

    void zconst::produce_node(std::ostream& output, regmap_t* map) const {
        this->rstr = zhvm::RA;
        if (this->level() != 0) {
            this->rstr = freereg(map);
        }
        output << zhvm::GetRegisterName(this->rstr) << " = add[," << this->value << "]" << std::endl;
    }

    int zconst::result() const {
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

    void zbinop::produce_node(std::ostream& output, regmap_t* map) const {
        this->right->produce_node(output, map);
        this->left->produce_node(output, map);
        this->rst = zhvm::RA;
        if (this->level() != 0) {
            this->rst = freereg(map);
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
        output << zhvm::GetRegisterName(this->rst) << " = " << optext << "[" << zhvm::GetRegisterName(this->left->result()) << "," << zhvm::GetRegisterName(this->right->result()) << "]" << std::endl;

        *map = resetused(*map, this->left->result());
        *map = resetused(*map, this->right->result());

    }

    int zbinop::result() const {
        return this->rst;
    }

    void zbinop::inc() {
        node::inc();
        left->inc();
        right->inc();
    }

    zbinop::zbinop(opid id, std::shared_ptr<node> left, std::shared_ptr<node> right) : node(0), id(id), left(left), right(right), rst(0) {
        this->left->inc();
        this->right->inc();
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

    void zinline::produce_node(std::ostream& output, regmap_t* map) const {
        output << this->text << std::endl;
    }

    int zinline::result() const {
        return -1;
    }

    zinline::zinline() : node(0), text() {
        ;
    }

    zinline::zinline(const std::string& text) : node(0), text(text) {
        ;
    }

    zinline::zinline(const char* text) : node(0), text(text) {
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

    void zprint::produce_node(std::ostream& output, regmap_t* map) const {
        this->item->produce_node(output, map);
        if (this->level() != 0) {
            throw std::runtime_error("Print only zero level node");
        }
        output << "$a = add[" << zhvm::GetRegisterName(this->item->result()) << "]" << std::endl;
        output << "cll[,0]" << std::endl;
        *map = resetused(*map, this->item->result());
    }

    int zprint::result() const {
        return -1;
    }

    void zprint::inc() {
        node::inc();
        this->item->inc();
    }

    zprint::zprint(std::shared_ptr<node> item) : node(0), item(item) {
        item->inc();
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