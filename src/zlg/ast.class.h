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

namespace zlg {

    typedef uint32_t regmap_t;

    class node {
        uint32_t lvl;

    public:
        virtual void print(FILE* output) const = 0;

        virtual void produce(FILE* output, regmap_t* map) = 0;

        virtual const char* result() const = 0;

        uint32_t level();
        virtual void inc();

        node(uint32_t level);

        node(const node& src);

        virtual ~node();

        node& operator=(const node& src);
    };

    class zconst : public node {
        int64_t value;
        const char* rstr;
    public:

        void print(FILE* output) const;

        void produce(FILE* output, regmap_t* map);

        const char* result() const;

        zconst();

        zconst(int64_t value);

        zconst(const zconst& src);

        ~zconst();

        zconst& operator=(const zconst& src);

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
        const char* rst;

    public:

        void print(FILE* output) const;

        void produce(FILE* output, regmap_t* map);

        const char* result() const;

        void inc();

        zbinop(opid id, std::shared_ptr<node> left, std::shared_ptr<node> right);

        zbinop(const zbinop& src);

        ~zbinop();

        zbinop& operator=(const zbinop& src);

    };

    class ast {
        std::list<std::shared_ptr<node> > items;
    public:

        void Print() const;

        void AddItem(std::shared_ptr<node> item);

        const std::list<std::shared_ptr<node> >& Items();

        ast();

        ast(const ast& orig);

        virtual ~ast();

        ast& operator=(const ast& src);
    };

}

#endif /* __AST_CLASS_HEADER__ */

