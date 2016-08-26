#pragma once
#ifndef __COMPILER_VER_2__
#define __COMPILER_VER_2__

#include <cstdint>

namespace zhvm {

#define ZHVM_MAX_CMPL_ID (8)

  enum token_type_v2 {
    TT2_ERROR = -1,
    TT2_EOF = 0,
    TT2_REG,
    TT2_SET,
    TT2_WORD,
    TT2_OPEN,
    TT2_CLOSE,
    TT2_COMMA,
    TT2_SIGN_MINUS,
    TT2_SIGN_PLUS,
    TT2_NUMBER_BYTE,
    TT2_NUMBER_SHORT,
    TT2_NUMBER_LONG,
    TT2_NUMBER_QUAD,
    TT2_MACRO,
    TT2_AT
  };
  
  struct token_reg_v2 {
    token_type_v2 type;
    uint32_t val;
  };
  
  struct token_opr_v2 {
    token_type_v2 type;
    char val[ZHVM_MAX_CMPL_ID];
  };
  
  struct token_num_v2 {
    token_type_v2 type;
    int val;
  };
  
  union token_v2 {
    token_type_v2 type;
    token_reg_v2 reg;
    token_opr_v2 opr;
    token_num_v2 num;
  };

  typedef int location;


#define YYSTYPE zhvm::token_v2
#define YYLTYPE zhvm::location
  
  struct yydata {
    YYSTYPE tok;
    YYLTYPE loc;
  };
  
  void ErrorMsg(location loc, const char* format, ...);
  void LogMsg(const char* format, ...);

}


#define ERROR_MSG(format, ...) zhvm::ErrorMsg(*yylloc, format, ##__VA_ARGS__)

#endif // __COMPILER_VER_2__
