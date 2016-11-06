# High-level language

ZLang - imperative high-level language which compiles to ZHVM assembly. Special 
words in ZLang are taken from esperanto.

## Reserved words

funk, es, alie, dum, presi

## Types

Current implementation supports only signed 64-bit integer numbers.

## Example

```
[
  a = 100
  b = 10
 
  dum (a > b) [
    a = a - 1
    presi a
  ]

  es ( b > 0 ) 
    presi a
  alie
    presi b

]
```

