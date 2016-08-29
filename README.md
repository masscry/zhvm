ZH Virtual Machine
==================

Overview
--------

ZHVM - is a registers based RISC virtual machine with 32-bit code length
and 64-bit registers.


Requirements
------------

ZHVM based on standard C library and STL.

Project structure
-----------------

ZHVM consists of two parts:

* libzhvm.a - library implements VM and assembler
* libzyaml.a - library implements JSON/YAML parsing
* repl - Read-Eval-Print-Loop application to work with ZHVM 
* cmplv2 - Standalone ZHVM assembler

Compilation
-----------

ZHVM project requires CMake 3.0 to be built.

Examples
--------

There are some ZHVM code examples in ${SOURCE_DIRECTORY}/share.

* test-001.zsf - simple consters program
* test-002.zsf - fibbonachi numbers calculator
* test-003.zsf - simple function invocation 