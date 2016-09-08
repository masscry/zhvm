Performance testing
===================

This directory contains test implementations of fibbonachi sequence calculation
in various programming languages.

Test case assumes calling function fib(30) and measure time needed to perform
calculations.

Current test languages:

* C++ - test.cpp
* Java - test.java
* Python - test.py
* Lua - test.lua

Actual performance table
------------------------

| System               | Time   | 
|----------------------|--------|
| g++ -O2 -m32         | 0.0068 |
| java                 | 0.008  |
| clang++ -O2 -m32     | 0.0122 |
| java -Xint           | 0.214  |
| lua                  | 0.27   |
| python               | 0.42   |
| zhvm clang++ BURST   | 0.63   |
| zhvm g++ BURST       | 0.70   |
| zhvm clang++ EXEC    | 0.86   |
| zhvm g++ EXEC        | 1.08   |
