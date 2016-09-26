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

        virtual void prepare_node() = 0;
        virtual void produce_node(std::ostream& output, regmap_t* map) const = 0;
        virtual int result() const;
        virtual void setResult(int nr) const;

        void Produce(std::ostream& output) const;

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

        void prepare_node();

        void produce_node(std::ostream& output, regmap_t* map) const;

        zconst();

        zconst(int64_t value);

        zconst(const zconst& src);

        ~zconst();

        zconst& operator=(const zconst& src);

    };

    class zinline : public node {
        std::string text;

    public:
        void prepare_node();
        void produce_node(std::ostream& output, regmap_t* map) const;

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
            DIV
        };

    private:

        opid id;
        std::shared_ptr<node> left;
        std::shared_ptr<node> right;

    public:

        void prepare_node();
        void produce_node(std::ostream& output, regmap_t* map) const;

        zbinop(opid id, std::shared_ptr<node> left, std::shared_ptr<node> right);

        zbinop(const zbinop& src);

        ~zbinop();

        zbinop& operator=(const zbinop& src);

    };

    class zprint : public node {
        std::shared_ptr<node> item;
    public:
        void prepare_node();
        void produce_node(std::ostream& output, regmap_t* map) const;

        zprint(std::shared_ptr<node> left);
        zprint(const zprint& src);
        ~zprint();

        zprint& operator=(const zprint& src);

    };

    class zprev : public node {
    public:
        void prepare_node();
        void produce_node(std::ostream& output, regmap_t* map) const;

        zprev();
        zprev(const zprint& src);
        ~zprev();

        zprev& operator=(const zprev& src);

    };

    class ast {
        std::list<std::shared_ptr<node> > items;
    public:

        void Scan();

        void Scan(const char* text);

        void Prepare();

        void Produce(std::ostream& output) const;

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

