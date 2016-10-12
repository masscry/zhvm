/** 
 * @file ast.class.h
 * @author marko
 * @date 18.09.2016
 */

#ifndef __AST_CLASS_HEADER__
#define __AST_CLASS_HEADER__

#include <memory>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <cassert>
#include <list>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <context.class.h>

namespace zlg {

    typedef context regmap_t;

    class node {
        uint32_t ershov;
        mutable int rstr;

    public:

        virtual void prepare_node(regmap_t* map) = 0;
        virtual void produce_node(std::ostream& output, regmap_t* map, int verbose) const = 0;
        virtual int result() const;
        virtual void setResult(int nr) const;

        void Produce(std::ostream& output, regmap_t* map, int verbose) const;

        node();

        node(const node& src);

        virtual ~node();

        node& operator=(const node& src);

        void setErshov(uint32_t ershov);
        uint32_t Ershov() const;
    };

    class zconst : public node {
        int64_t value;
    public:

        void prepare_node(regmap_t* map);
        void produce_node(std::ostream& output, regmap_t* map, int verbose) const;

        zconst();

        zconst(int64_t value);

        zconst(const zconst& src);

        ~zconst();

        zconst& operator=(const zconst& src);

    };

    class zinline : public node {
        std::string text;

    public:
        void prepare_node(regmap_t* map);
        void produce_node(std::ostream& output, regmap_t* map, int verbose) const;

        zinline();

        zinline(const std::string& text);
        zinline(const char* text);

        zinline(const zinline& src);

        ~zinline();

        zinline& operator=(const zinline& src);

    };

    class zbinop : public node {
    public:

        enum opid {
            UNDEF,
            ADD,
            SUB,
            MUL,
            DIV,
            SET
        };

    private:

        opid id;
        std::shared_ptr<node> left;
        std::shared_ptr<node> right;
        
        static const char* OPIDString(opid id);

    public:

        void prepare_node(regmap_t* map);
        void produce_node(std::ostream& output, regmap_t* map, int verbose) const;

        zbinop(opid id, std::shared_ptr<node> left, std::shared_ptr<node> right);

        zbinop(const zbinop& src);

        ~zbinop();

        zbinop& operator=(const zbinop& src);

    };

    class zprint : public node {
        std::shared_ptr<node> item;
    public:
        void prepare_node(regmap_t* map);
        void produce_node(std::ostream& output, regmap_t* map, int verbose) const;

        zprint(std::shared_ptr<node> left);
        zprint(const zprint& src);
        ~zprint();

        zprint& operator=(const zprint& src);

    };

    class zprev : public node {
    public:
        void prepare_node(regmap_t* map);
        void produce_node(std::ostream& output, regmap_t* map, int verbose) const;

        zprev();
        zprev(const zprev& src);
        ~zprev();

        zprev& operator=(const zprev& src);

    };

    class zvar : public node {
        std::string id;

    public:

        void prepare_node(regmap_t* map);
        void produce_node(std::ostream& output, regmap_t* map, int verbose) const;

        zvar(const char* id);
        zvar(const zvar& src);
        ~zvar();

        zvar& operator=(const zvar& src);

        const std::string& VarID() const;

    };

    class zerror : public node {
    public:

        void prepare_node(regmap_t* map) {
            this->setErshov(1);
        }

        void produce_node(std::ostream& output, regmap_t* map, int verbose) const {
            output << "# =Syntax error=" << std::endl;
            output << "nop[]" << std::endl;
        }

        zerror() {
            ;
        }

        zerror(const zvar& src) {
            ;
        }

        ~zerror() {
            ;
        }

        zerror& operator=(const zerror& src) {
            return *this;
        }

    };

    class ast {
        std::list<std::shared_ptr<node> > items;
    public:

        void Scan();

        void Scan(const char* text);

        void Generate(std::ostream& output, context* map, int verbose);

        void AddItem(std::shared_ptr<node> item);

        const std::list<std::shared_ptr<node> >& Items() const;

        ast();

        ast(const ast& orig);

        virtual ~ast();

        ast& operator=(const ast& src);

        ast& operator=(ast&& src);

    };

}

#endif /* __AST_CLASS_HEADER__ */

