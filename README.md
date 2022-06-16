# eBPF-HLS
This is the repository of my undergraduate final project. It has been conducted at the University of Colorado Boulder, at Boulder, US. It has been directed by Professor Eric Keller and Professor Tamara Silbergleit Lehman, from the Electrical, Computer & Energy Engineering (ECEE) Department.\
The goal of this repository is to store the source code of modified eBPF programs that have been used in the performance analysis. They can be synthesized using Vitis HLS.\
This repository has a directory for each function or program, and each directory follows the same structure:
- src: it contains the source Restricted C code and headers.
- test: it contains the source test C code and headers.
- def: it can only be found in some directories, and it contains a C (or Restricted C) header file with definitions.\
Regarding the files, there are six different directories:
- 00_Common_Libraries: library with common eBPF functions.
- Adapter_PING: program that manages ICMPv4 packets and that has been loaded to the NetFPGA-SUME.
- XDP1: program that parses and discards packets.
- XDP2: program that parses and forwards packets.
- XDP_ADJUST_TAIL: program that parses and modifies the size of packets.
- Bitfile: file that contains a design with the Adapter_PING program integrated, and that can be directly loaded to a NetFPGA-SUME.
