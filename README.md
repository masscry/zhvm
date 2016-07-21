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
* repl - Read-Eval-Print-Loop application to work with ZHVM 

Compilation
-----------

ZHVM project requires CMake 3.0 to be built.
