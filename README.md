<p align="center">
  <img src="resources/banner.png" width="450vw" height="200vw"/>
</p>

<h2 align="center">A simple, multiplatform, Sony Playstation 1 emulator written in C</h2>

<p align="center">
  <img src="https://github.com/cakehonolulu/SimpleStation/actions/workflows/simplestation.yml/badge.svg">
  <img src="https://byob.yarr.is/cakehonolulu/SimpleStation/build">
</p>

<table align="center">
<h2 align="center">PSX Bootlogo</h2>
<thead>
  <tr>
    <td><h4>PAL PSX</h4><img src="https://raw.githubusercontent.com/cakehonolulu/SimpleStation/main/resources/bootup.png" width="250vw" height="185vh" alt="Image"></td>
    <td><h4>NTSC PSX</h4><img src="https://raw.githubusercontent.com/cakehonolulu/SimpleStation/main/resources/bootup2.png" width="250vw" height="185vh" alt="Image"></td>
    <td><h4>Experimental Vulkan Renderer</h4><img src="https://raw.githubusercontent.com/cakehonolulu/SimpleStation/main/resources/vulkan.png" width="250vw" height="185vh" alt="Image"></td>
  </tr>
</thead>
</table>

<table align="center">
<h2 align="center">PSX Menus</h2>
<thead>
  <tr>
    <td><h4>BIOS Menu</h4><img src="https://raw.githubusercontent.com/cakehonolulu/SimpleStation/main/resources/bios.png" width="250vw" height="185vh" alt="Image"></td>
    <td><h4>Another BIOS Menu</h4><img src="https://raw.githubusercontent.com/cakehonolulu/SimpleStation/main/resources/bios2.png" width="250vw" height="185vh" alt="Image"></td>
    <td><h4>PSX Game Boot</h4><img src="https://raw.githubusercontent.com/cakehonolulu/SimpleStation/main/resources/psx_logo.png" width="250vw" height="185vh" alt="Image"></td>
  </tr>
</thead>
</table>

<table align="center">
<h2 align="center">PSX Demos</h2>
<thead>
  <tr>
    <td><h4>Amidog CPU Tests</h4><img src="https://raw.githubusercontent.com/cakehonolulu/SimpleStation/main/resources/amidog.png" width="250vw" height="185vh" alt="Image"></td>
    <td><h4>Quads Demo</h4><img src="https://raw.githubusercontent.com/cakehonolulu/SimpleStation/main/resources/quad.png" width="250vw" height="185vh" alt="Image"></td>
    <td><h4>Goraud Triangle Demo</h4><img src="https://raw.githubusercontent.com/cakehonolulu/SimpleStation/main/resources/triangle.png" width="250vw" height="185vh" alt="Image"></td>
  </tr>
</thead>
</table>

## ✨ Features
* Targets [C23](https://en.cppreference.com/w/c/23) features (Fallbacks present if not available)
* LLE Interpreter
* Multiplatform
* Multiarchitecture
* Hardware Renderer (OpenGL 3.3, and an experimental Vulkan renderer)

## 🖥️ Current state
#### CPU Subsystem
- All opcodes implemented (100%)
  - Regular, e(X)tended, Jump and Branch Opcodes
  - MIPS Coprocessor 0 Opcodes
  - MIPS 5-stage Instruction Pipeline Emulation
  - Readable-format disassembler with debugger and breakpoint support
  - Custom gdbstub implementation to debug PSX code with gdb
- No cache emulation (_Yet!_)

#### GPU Subsystem
- Sony's Propietary GPU
  - _In the works_
- Sony's Coprocessor 2 "GTE" (Geometry Transformation Engine)
  - _In the works_

#### DMA Subsystem
- DMA Unit
  - _In the works_

#### CDROM Subsystem
- CDROM Unit
  - _In the works_

#### BIOS Compatibility
- BIOS41A.bin	| b9d9a0286c33dc6b7237bb13cd46fdee
	- Boots to badly-rendered shell
- SCPH101.bin	| 6e3735ff4c7dc899ee98981385f6f3d0
	- Boots to shell
- SCPH1000.bin	| 239665b1a3dade1b5a52c06338011044
	- Graphical glitches on boot logo, crashes due to missing CDROM commands
- SCPH1001.bin	| 924e392ed05558ffdb115408c263dccf
	- Preferred BIOS by the emulator developer, boots to shell
- SCPH1002.bin	| 54847e693405ffeb0359c6287434cbef
	- Infinite loop after boot logo (Right before shell)
- SCPH3000.bin	| 849515939161e62f6b866f6853006780
	- Graphical glitches on boot logo, boots to shell
- SCPH3500.bin	| cba733ceeff5aef5c32254f1d617fa62
	- Boots to shell
- SCPH5000.bin	| 57a06303dfa9cf9351222dfcbb4a29d9
	- Boots to shell
- SCPH5500.bin	| 8dd7d5296a650fac7319bce665a6a53c
	- Boots to shell
- SCPH5502.bin	| e56ec1b027e2fe8a49217d9678f7f6bb
	- Nothing displayed, Illegal Opcode 0x0053DE3E
- SCPH5552.bin	| 32736f17079d0b2b7024407c39bd3050
	- Infinite loop after boot logo (Right before shell)
- SCPH7000.bin	| 8e4c14f567745eff2f0408c8129f72a6
	- Boots to shell
- SCPH7001.bin	| 1e68c231d0896b7eadcad1d7d8e76129
	- Boots to shell
- SCPH7003.bin	| 490f666e1afb15b7362b406ed1cea246
	- Boots to shell
- SCPH7502.bin	| b9d9a0286c33dc6b7237bb13cd46fdee
	- Boots to badly-rendered shell

## 👷🏼‍♂️ Roadmap
* MIPS I R3000A to x86_64 (AMD64) Dynamic Recompilation (JIT)

## 🪐 Special Agreements
- @wheremyfoodat
- @iuk7071

## 📃 Documentation
* Integrated Device Technology, Inc. (1996). _IDT MIPS Microprocessor Family Software Reference Manual_. Wayback Machine. [https://web.archive.org/web/20061010210946/http://decstation.unix-ag.org/docs/ic_docs/3715.pdf](https://web.archive.org/web/20061010210946/http://decstation.unix-ag.org/docs/ic_docs/3715.pdf)
* Flandrin, L. (2016, 20 October). _Playstation Emulation Guide_. svkt.org. [https://svkt.org/~simias/guide.pdf](https://svkt.org/~simias/guide.pdf)
* PlayStation Specifications - psx-spx. (2022). PSX SPX Consoledev. [https://psx-spx.consoledev.net/](https://psx-spx.consoledev.net/)
* Copetti, R. (2022, December 24). PlayStation Architecture. The Copetti Site. [https://www.copetti.org/writings/consoles/playstation/]
