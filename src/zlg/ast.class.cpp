/** 
 * @file ast.class.cpp
 * @author marko
 * @date 18.09.2016
 */

#include <zlg.h>
#include <zhvm.h>
#include <zlgy.gen.hpp>
#include <zlg.gen.h>

namespace zlg {

    void ast::Scan() {
        yyscan_t scan;

        zlg::ast temp;

        yylex_init(&scan);

        try {
            yyparse(scan, temp);
        } catch (std::exception& err) {
            std::cerr << err.what() << std::endl;
        }

        yylex_destroy(scan);
        *this = std::move(temp);
    }

    void ast::Scan(const char* text) {
        yyscan_t scan;

        zlg::ast temp;

        yylex_init(&scan);
        YY_BUFFER_STATE input = yy_scan_string(text, scan);

        try {
            yyparse(scan, temp);
        } catch (std::exception& err) {
            std::cerr << err.what() << std::endl;
        }

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

    void ast::AddItem(node_p item) {
        this->items.push_back(item);
    }

    const std::list<node_p >& ast::Items() const {
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