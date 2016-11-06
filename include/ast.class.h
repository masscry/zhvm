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

    typedef std::shared_ptr<node> node_p;
    
    class znull : public node {        
    public:
        void prepare_node(regmap_t* map){;}
        void produce_node(std::ostream& output, regmap_t* map, int verbose) const{
            output << "nop[]" << std::endl;
        }
        znull(){;}
        znull(const znull& src){;}
        ~znull(){;}
        znull& operator=(const znull& src){ return *this;}            
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

        static const char* OPIDString(opid id);

    public:

        void prepare_node(regmap_t* map);
        void produce_node(std::ostream& output, regmap_t* map, int verbose) const;

        zbinop(opid id, node_p left, node_p right);

        zbinop(const zbinop& src);

        ~zbinop();

        zbinop& operator=(const zbinop& src);

    };

    class zunop : public node {
    public:

        enum opid {
            UNDEF,
            PLUS,
            MINUS,
            NOT
        };

    private:

        opid id;
        node_p right;

        static const char* OPIDString(opid id);

    public:

        void prepare_node(regmap_t* map);
        void produce_node(std::ostream& output, regmap_t* map, int verbose) const;

        zunop(opid id, node_p right);

        zunop(const zunop& src);

        ~zunop();

        zunop& operator=(const zunop& src);

    };

    class zprint : public node {
        node_p item;
    public:
        void prepare_node(regmap_t* map);
        void produce_node(std::ostream& output, regmap_t* map, int verbose) const;

        zprint(node_p left);
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

    class zblock : public node {
        std::list<node_p> items;

    public:

        void add_item(node_p item);

        void prepare_node(regmap_t* map);
        void produce_node(std::ostream& output, regmap_t* map, int verbose) const;

        zblock();
        zblock(const zblock& src);
        ~zblock();

        zblock& operator=(const zblock& src);

    };

    class zif : public node {
        node_p cond;
        node_p trueb;

        uint32_t uid;

    public:

        void prepare_node(regmap_t* map);
        void produce_node(std::ostream& output, regmap_t* map, int verbose) const;

        zif(node_p cond, node_p trueb);
        zif(const zif& src);
        ~zif();

        zif& operator=(const zif& src);

    };

    class zifelse : public node {
        node_p cond;
        node_p trueb;
        node_p falseb;

        uint32_t uid;

    public:

        void prepare_node(regmap_t* map);
        void produce_node(std::ostream& output, regmap_t* map, int verbose) const;

        zifelse(node_p cond, node_p trueb, node_p falseb);
        zifelse(const zifelse& src);
        ~zifelse();

        zifelse& operator=(const zifelse& src);

    };

    class zwhile : public node {
        node_p cond;
        node_p trueb;

        uint32_t uid;

    public:

        void prepare_node(regmap_t* map);
        void produce_node(std::ostream& output, regmap_t* map, int verbose) const;

        zwhile(node_p cond, node_p trueb);
        zwhile(const zwhile& src);
        ~zwhile();

        zwhile& operator=(const zwhile& src);

    };

    class ast {
        std::list<node_p> items;
    public:

        void Scan();

        void Scan(const char* text);

        void Generate(std::ostream& output, context* map, int verbose);

        void AddItem(node_p item);

        const std::list<node_p>& Items() const;

        ast();

        ast(const ast& orig);

        virtual ~ast();

        ast& operator=(const ast& src);

        ast& operator=(ast&& src);

    };

}

#endif /* __AST_CLASS_HEADER__ */

