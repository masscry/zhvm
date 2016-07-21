/**
 * @file sman.h
 * @author marko
 *
 * String manipulations
 *
 */

#pragma once
#ifndef __SMAN_HEADER__
#define __SMAN_HEADER__

#include <cstdint>

namespace zhvm {

  typedef const char cchar;

  /**
   * Move cursor forward until first non-space character or \0 found.
   *
   * @param cursor string cursor
   * @return cursor at first non-space character or \0
   */
  cchar* SkipSpace(cchar* cursor);

  /**
   * Skip spaces and check if expected character found after.
   *
   * @param smb expected character
   * @param cursor string cursor
   * @return cursor after expected character, \0 or 0 if wrong character found
   */
  cchar* ExpectSymbol(char smb, cchar* cursor);


  /**
   * Skip spaces and check if next two symbols are in form [$][a-cA-C0-8sSdDpPzZ].
   *
   * @param cursor string cursor
   * @param result pointer to expected register ID
   * @return string cursor after expected register or at same position
   * @see registers
   */
  cchar* ExpectRegister(cchar* cursor, int32_t* result);


  /**
   * Copy non-whitespace character sequence from cursor position to buffer.
   *
   * @param cursor string cursor
   * @param result output buffer
   * @param maxresult output buffer size
   * @return cursor after copied sequence, or at last copied symbol if output buffer is too small
   */
  cchar* ExtractText(cchar* cursor, char* result, size_t maxresult);

  /**
   * Skip spaces and get opcode ID.
   *
   * @param cursor string cursor
   * @param result found opcode ID
   * @return cursor after extracted opcode
   */
  cchar* ExpectOpcode(cchar* cursor, int32_t* result);

  /**
   * Assemble VM code in form "$X opcode $Y, $Z, IMM".
   *
   * @param cursor string cursor
   * @param result result code in binary form
   * @return 0 on error, cursor after processed code
   */
  cchar* Assemble(cchar* cursor, cmd* result);



  /**
   * Return register name by its ID.
   *
   * @param reg register ID.
   * @return null-terminated string of register name
   * @see register
   */
  cchar* GetRegisterName(uint32_t reg);


}

#endif // __SMAN_HEADER__

