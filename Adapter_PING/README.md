# Adapter PING
This directory contains a program that has been loaded to the NetFPGA-SUME. It includes adapters to make the design work in the board. The eBPF program consists of a simple function that parses the packet up to the ICMPv4 header, and it drops it if it has an even sequence number. Otherwise, it lets it pass.
