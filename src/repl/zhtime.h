/**
 * @file zhtime.h
 * @author timur
 *
 * Time measurement functions 
 *
 */

#pragma once
#ifndef __ZHTIME_HEADER__
#define __ZHTIME_HEADER__

#ifdef WIN32
#include <Windows.h>
#undef RC_NONE
#endif // WIN32

#ifdef UNIX
#include <ctime>
#endif // UNIX

namespace zhvm {
#ifdef WIN32
  typedef LARGE_INTEGER TD_TIME;
#endif // WIN32
#ifdef UNIX
  typedef timespec TD_TIME;
#endif // UNIX

  double time_diff(const TD_TIME& start, const TD_TIME& stop);
  
  void zhtime(TD_TIME* td);
}

#endif // __ZHTIME_HEADER__