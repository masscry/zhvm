#include <zyaml.h>
#include <zyaml.gen.h>

#include <stack>

enum zyaml_parser_mode {
    ZM_BAD_END,
    ZM_EXPECT_VALUE,
    ZM_PARSE_LIST,
    ZM_PARSE_LIST_NEXT_ITEM,
    ZM_PARSE_DICT,
    ZM_PARSE_DICT_EXPECT_KEY,
    ZM_PARSE_DICT_EXPECT_VALUE,
    ZM_PARSE_DICT_NEXT_ITEM,
    ZM_PARSE_WORD,
    ZM_PARSE_INTEGER,
    ZM_PARSE_NUMBER,
};

namespace {

    static const char* ztt_strings[ZTT_TOTAL + 1] = {
        "ERROR",
        "EOF",
        "START STREAM",
        "END STREAM",
        "START LIST",
        "END LIST",
        "START DICT",
        "END DICT",
        "WORD",
        "KEY",
        "INTEGER",
        "NUMBER",
        "COMMA",
        "COLON"
    };

}

const char* zyaml_token_type_string(int type) {
    if ((type >= -1)&&(type < ZTT_TOTAL + 1)) {
        return ztt_strings[type + 1];
    }
    return "UNDEF";
}

int zyaml(FILE* input, zyaml_callback callback, void* data) {

    yyscan_t context;
    yylex_init(&context);
    yyset_in(input, context);

    YYSTYPE token = {0};
    YYLTYPE line = 0;

    std::stack<int> mode;

    int ctok = yylex(&token, &line, context);

#define NEXT_TOKEN ctok = yylex(&token, &line, context);
#define RETURN_ERROR(error) token.line = line; token.text = (error); token.type = ZTT_ERROR; callback(&token, data); mode.push(ZM_BAD_END)
#define RETURN_TOKEN if (callback(&token, data) == 0) { token.line = line; NEXT_TOKEN; } else { mode.push(ZM_BAD_END); }
#define RETURN_POP_TOKEN if (callback(&token, data) == 0) { token.line = line; NEXT_TOKEN; mode.pop(); } else { mode.push(ZM_BAD_END); }

    mode.push(ZM_EXPECT_VALUE);

    while ((!mode.empty())&&(mode.top() != ZM_BAD_END)&&(ctok != ZTT_EOF)) {

        switch (mode.top()) {
            case ZM_EXPECT_VALUE:
            {
                switch (ctok) {
                    case ZTT_LIST_START:
                        mode.top() = ZM_PARSE_LIST;
                        RETURN_TOKEN;
                        break;
                    case ZTT_DICT_START:
                        mode.top() = ZM_PARSE_DICT;
                        RETURN_TOKEN;
                        break;
                    case ZTT_WORD:
                        mode.top() = ZM_PARSE_WORD;
                        break;
                    case ZTT_INTEGER:
                        mode.top() = ZM_PARSE_INTEGER;
                        break;
                    case ZTT_NUMBER:
                        mode.top() = ZM_PARSE_NUMBER;
                        break;
                    default:
                        RETURN_ERROR("VALUE EXPECTED");
                }
                break;
            }
            case ZM_PARSE_LIST:
            {
                switch (ctok) {
                    case ZTT_LIST_END:
                        RETURN_POP_TOKEN;
                        break;
                    case ZTT_LIST_START:
                    case ZTT_DICT_START:
                    case ZTT_WORD:
                    case ZTT_INTEGER:
                    case ZTT_NUMBER:
                        mode.top() = ZM_PARSE_LIST_NEXT_ITEM;
                        mode.push(ZM_EXPECT_VALUE);
                        break;
                    default:
                        RETURN_ERROR("] OR VALUE EXPECTED");
                }
                break;
            }
            case ZM_PARSE_LIST_NEXT_ITEM:
            {
                switch (ctok) {
                    case ZTT_LIST_END:
                        RETURN_POP_TOKEN;
                        break;
                    case ZTT_COMMA:
                        mode.top() = ZM_PARSE_LIST;
                        NEXT_TOKEN;
                        break;
                    default:
                        RETURN_ERROR("] OR , EXPECTED");
                }
                break;
            }
            case ZM_PARSE_DICT:
            {
                switch (ctok) {
                    case ZTT_DICT_END:
                        RETURN_POP_TOKEN;
                        break;
                    case ZTT_LIST_START:
                    case ZTT_DICT_START:
                    case ZTT_WORD:
                    case ZTT_INTEGER:
                    case ZTT_NUMBER:
                        mode.top() = ZM_PARSE_DICT_EXPECT_VALUE;
                        mode.push(ZM_PARSE_DICT_EXPECT_KEY);
                        break;
                    default:
                        RETURN_ERROR("] OR WORD EXPECTED");
                }
                break;
            }
            case ZM_PARSE_DICT_EXPECT_KEY:
            {
                switch (ctok) {
                    case ZTT_LIST_START:
                    case ZTT_DICT_START:
                    case ZTT_INTEGER:
                    case ZTT_NUMBER:
                        RETURN_ERROR("ONLY WORD KEYS ALLOWED");
                        break;
                    case ZTT_WORD:
                        token.type = ZTT_WORD_KEY;
                        RETURN_POP_TOKEN;
                        break;
                    default:
                        RETURN_ERROR("WORD EXPECTED");
                }
                break;
            }
            case ZM_PARSE_DICT_EXPECT_VALUE:
            {
                switch (ctok) {
                    case ZTT_COLON:
                        mode.top() = ZM_PARSE_DICT_NEXT_ITEM;
                        mode.push(ZM_EXPECT_VALUE);
                        NEXT_TOKEN;
                        break;
                    default:
                        RETURN_ERROR("COLON EXPECTED");
                }
                break;
            }
            case ZM_PARSE_DICT_NEXT_ITEM:
            {
                switch (ctok) {
                    case ZTT_DICT_END:
                        RETURN_POP_TOKEN;
                        break;
                    case ZTT_COMMA:
                        mode.top() = ZM_PARSE_DICT;
                        NEXT_TOKEN;
                        break;
                    default:
                        RETURN_ERROR("} OR , EXPECTED");
                }
                break;
            }
            case ZM_PARSE_WORD:
            case ZM_PARSE_INTEGER:
            case ZM_PARSE_NUMBER:
            {
                RETURN_POP_TOKEN;
                break;
            }
            default:
                RETURN_ERROR("INVALID INTERNAL PARSER STATE");
        }
    }

    yylex_destroy(context);

    if (mode.empty()) {
        return ZTT_EOF;
    }

    return ZTT_ERROR;
}