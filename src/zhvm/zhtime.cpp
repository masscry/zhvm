#include "zhtime.h"

#ifdef WIN32 

namespace {

  class sysfreq {
    LARGE_INTEGER self;
    
    sysfreq():self() {
      QueryPerformanceFrequency(&self);
    }

  public:

    const LARGE_INTEGER& freq() const
    {
      return this->self;
    }

    static sysfreq& Instance()
    {
      static sysfreq self;
      return self;
    }

  };

}

#endif // WIN32


namespace zhvm {  

#ifdef WIN32

  void zhtime(TD_TIME* td) {
    QueryPerformanceCounter(td);
  }

  double time_diff(const TD_TIME& start, const TD_TIME& stop) {
    TD_TIME passed;
    passed.QuadPart = stop.QuadPart - start.QuadPart;
    return (double)passed.QuadPart/(double)sysfreq::Instance().freq().QuadPart;
  }

#endif // WIN32

#ifdef UNIX

  void zhtime(TD_TIME* td) {
     clock_gettime(CLOCK_PROCESS_CPUTIME_ID, td);
  }

  double time_diff(const TD_TIME& start, const TD_TIME& stop) {
    TD_TIME temp;
    if (stop.tv_nsec < start.tv_nsec) {
        temp.tv_sec = stop.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + stop.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = stop.tv_sec - start.tv_sec;
        temp.tv_nsec = stop.tv_nsec - start.tv_nsec;
    }
    return (double) temp.tv_sec + temp.tv_nsec / 1.0e9;
  }

#endif // UNIX

}


