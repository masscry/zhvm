High-level language
===================

ZHVM scripts can be developed in assembly, or in zlang. ZLang is imperative 
language, code can be splitted in functions. Algebraic expressions written with
infix notation.

Reserved words
--------------

fun, byte, short, long, quad, exit, result.

Types
-----

Zlang has 4 types:

* byte - 1-byte integer
* short - 2-byte integer
* long - 4-byte integer
* quad - 8-byte integer

Example
-------

```
fun fib[long a]{
  if [a == 0] {
    result = 0
    exit
  }
  
  if [a == 1] {
    result = 1
    exit
  }
  
  result = fib[a-1] + fib[a-2]
}

fun entry[]{
  print[fib[5]]
}

```
