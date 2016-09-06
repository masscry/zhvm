import time

def fib(n):
    if (n == 0):
        return 0
    if (n == 1):
        return 1
    return fib(n-1) + fib(n-2)

def run():
    start = time.clock()
    result = fib(30)
    finish = time.clock()
    print 'RESULT: %d [%g]'%(result, finish-start)

if __name__ == '__main__':
    run()
