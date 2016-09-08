#!/usr/bin/lua

function fib(n)

    if (n == 0) then
      return 0
    end
      
    if (n == 1) then
      return 1
    end
      
    return fib(n-1) + fib(n-2)
end

local start = 0
local finish = 0
local result = 0

start = os.clock()
result = fib(30)
finish = os.clock()

print(string.format("RESULT: %d [%g]", result, finish-start))
