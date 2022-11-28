<p align="center">
  <img src="resources/banner.png" width="400" height="200"/>
</p>

<h2 align="center">A simple, multiplatform, Sony Playstation 1 (PSX) emulator written in C23</h2>





[![CodeQL](https://github.com/cakehonolulu/SimpleStation/actions/workflows/simplestation.yml/badge.svg)](https://github.com/cakehonolulu/SimpleStation/actions/workflows/simplestation.yml) ![](https://byob.yarr.is/cakehonolulu/SimpleStation/build)

## ✨ Features
* Built using [C23](https://en.cppreference.com/w/c/23) features
* LLE Interpreter Emulator
* Upstream compiler support (clang-16 & gcc-12)

## 🖥️ Specs
#### Main CPU (MIPS R3051; an R3000A-compatible 32-bit RISC CPU)
- [✅] 69 out of 69 instructions implemented (100%)
    - Extended and Branch Opcodes
    - MIPS Coprocessor 0 (28 out of 28 instructions implemented)
    - MIPS Instruction Pipeline Emulation
- [ ] Sony Coprocessor 2 (GTE, Geometry Transformation Engine) _in-the-works_
#### MIPS R3000 Disassembler
* Currently works on PSX's BIOS
#### Memory Bus
* Byte, Word, Double Word MMU Read/Write Handling
* Unaligned memory access handling
* Memory loads pipeline emulation
#### DMA Bus
* DMA subsystem _in the works_

## 👷🏼‍♂️ Roadmap
* MIPS I R3000A to x86_64 (AMD64) Dynamic Recompilation (JIT)

## 📃 Documentation
* Integrated Device Technology, Inc. (1996). _IDT MIPS Microprocessor Family Software Reference Manual_. Wayback Machine. [https://web.archive.org/web/20061010210946/http://decstation.unix-ag.org/docs/ic_docs/3715.pdf](https://web.archive.org/web/20061010210946/http://decstation.unix-ag.org/docs/ic_docs/3715.pdf)
* Flandrin, L. (2016, 20 October). _Playstation Emulation Guide_. svkt.org. [https://svkt.org/~simias/guide.pdf](https://svkt.org/~simias/guide.pdf)
* PlayStation Specifications - psx-spx. (2022). PSX SPX Consoledev. [https://psx-spx.consoledev.net/](https://psx-spx.consoledev.net/)
