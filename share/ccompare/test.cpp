#include <cstdio>
#include <ctime>

int fib(int a){
  if (a == 0) return 0;
  if (a == 1) return 1;
  return fib(a-1) + fib(a-2);
}

double time_diff(const timespec& start, const timespec& stop) {
  timespec temp;
  if (stop.tv_nsec < start.tv_nsec) {
    temp.tv_sec = stop.tv_sec - start.tv_sec - 1;
    temp.tv_nsec = 1000000000 + stop.tv_nsec - start.tv_nsec;
  } else {
    temp.tv_sec = stop.tv_sec - start.tv_sec;
    temp.tv_nsec = stop.tv_nsec - start.tv_nsec;
  }
  return (double) temp.tv_sec + temp.tv_nsec / 1.0e9;
}

int main(int argc, char* argv[]){

  timespec start;
  timespec finish;

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
  int result = fib(30);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &finish);
  
  printf("%s: %d [%g]\n", "RESULT", result, time_diff(start, finish));

  return 0;
}