#pragma once
#ifndef __ZHVM_LEXER__
#define __ZHVM_LEXER__

#include <cstdint>

namespace zhvm {

#define ZHVM_MAX_TOKEN_ID (8)

  enum token_type {
    TOK_ERROR = -1,
    TOK_EOF = 0,
    TOK_ID,
    TOK_SREG,
    TOK_EREG,
    TOK_NUMBER,
    TOK_SPACE,
    TOK_COMMA,
  };

  struct token_register {
    int32_t tok;
    uint32_t val;
  };

  struct token_number {
    int32_t tok;
    int64_t val;
  };

  struct token_id {
    int32_t tok;
    char val[ZHVM_MAX_TOKEN_ID];
  };

  union  token {
    int32_t tok;
    token_register reg;
    token_number num;
    token_id id;
  };
  
  struct location {
    int line;
    int col;
  };

}

#define YYSTYPE zhvm::token
#define YYLTYPE zhvm::location

#endif // __ZHVM_LEXER__
