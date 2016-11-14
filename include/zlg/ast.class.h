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
#include "context.class.h"
#include "node.class.h"

namespace zlg {

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

