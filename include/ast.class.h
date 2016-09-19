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

namespace zlg {

    typedef uint32_t regmap_t;

    class node {
        uint32_t lvl;

    public:

        virtual void produce_node(std::ostream& output, regmap_t* map) const = 0;
        virtual int result() const = 0;

        void Produce(std::ostream& output) const;

        uint32_t level() const;
        virtual void inc();

        node(uint32_t level);

        node(const node& src);

        virtual ~node();

        node& operator=(const node& src);
    };

    class zconst : public node {
        int64_t value;
        mutable int rstr;
    public:

        void produce_node(std::ostream& output, regmap_t* map) const;

        int result() const;

        zconst();

        zconst(int64_t value);

        zconst(const zconst& src);

        ~zconst();

        zconst& operator=(const zconst& src);

    };

    class zinline : public node {
        std::string text;

    public:

        void produce_node(std::ostream& output, regmap_t* map) const;
        int result() const;

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
        mutable int rst;

    public:

        void produce_node(std::ostream& output, regmap_t* map) const;

        int result() const;

        void inc();

        zbinop(opid id, std::shared_ptr<node> left, std::shared_ptr<node> right);

        zbinop(const zbinop& src);

        ~zbinop();

        zbinop& operator=(const zbinop& src);

    };

    class zprint : public node {
        std::shared_ptr<node> item;
    public:

        void produce_node(std::ostream& output, regmap_t* map) const;
        int result() const;
        void inc();

        zprint(std::shared_ptr<node> left);
        zprint(const zprint& src);
        ~zprint();

        zprint& operator=(const zprint& src);

    };

    class ast {
        std::list<std::shared_ptr<node> > items;
    public:

        void Scan();

        void Scan(const char* text);

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

