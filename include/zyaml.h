/**
 * @file zyaml.h
 * @author timur
 * 
 * SAX YAML parser based on FLEX.
 * 
 * Prefered syntax
 * ===============
 * 
 * @code
 * 
 * ---
 * # Comments
 * 
 * [ # ROOT OBJECT
 *   [ a, b, c ],     # list of items
 *   { a: b, c: d},   # dict of items
 *   one-word-string, # string in one word without braces
 *   "two words",     # two words string with braces
 *   123,             # integer
 *   123.4343         # floating point number
 * ]
 * ...
 * 
 * @endcode
 * 
 * 
 */

#pragma once
#ifndef __ZYAML_HEADER__
#define __ZYAML_HEADER__

#include <string>
#include <cstdint>

/**
 * Token types
 */
enum zyaml_token_type {
    ZTT_ERROR = -1, ///< Error
    ZTT_EOF = 0, ///< FLEX returns 0 when eof reached
    ZTT_FILE_START, ///< File starts
    ZTT_FILE_END, ///< File ends
    ZTT_LIST_START, ///< List starts
    ZTT_LIST_END, ///< List ends
    ZTT_DICT_START, ///< Dictionary starts
    ZTT_DICT_END, ///< Dictionary ends
    ZTT_WORD, ///< Word and multiword
    ZTT_WORD_KEY, ///< Word and multiword key
    ZTT_INTEGER, ///< Integer
    ZTT_NUMBER, ///< Floating point number
    ZTT_COMMA, ///< Comma
    ZTT_COLON, ///< Colon
    ZTT_TOTAL
};

/**
 * Get zyaml token type string
 * 
 * @param type token type ID.
 * @return token type string
 */
const char* zyaml_token_type_string(int type);

/**
 * Token
 */
struct zyaml_token {
    int type; ///< Token type
    std::string text; ///< Token text
    int line; ///< Token line in input stream
};

/**
 * Main callback function.
 * 
 * Callback returns 0 if everything is ok, -1 otherwise.
 */
typedef int (*zyaml_callback)(const zyaml_token* token, void* data);

/**
 * Main parsing function.
 * 
 * @param input input file
 * @param callback user provided callback function
 * @param data user data passed to callback
 * @return 0 if everything is ok, line where error occured, or -1 if not all braces closed or file opened.
 */
int zyaml(FILE* input, zyaml_callback callback, void* data);

/**
 * Token type definition for FLEX
 */
#define YYSTYPE zyaml_token

/**
 * Location type definition for FLEX
 */
#define YYLTYPE uint32_t



#endif // __ZYAML_HEADER__