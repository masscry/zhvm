#pragma once
#ifndef __ZHVM_LEXER__
#define __ZHVM_LEXER__

namespace zhvm {

  enum lex_token {
    LT_EOF,
    LT_ID,
    LT_SREG,
    LT_EREG,
    LT_NUMBER,
    LT_SPACE,
    LT_COMMA,
  };

  union lex_result {
    int lt;
    
  }


}

#endif // __ZHVM_LEXER__