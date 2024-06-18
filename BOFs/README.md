# Beacon Object Files (BOFs)
All beacon object files given with the framework, the rest you have to create on your own.

### OutputFormatter.h
If your BOF wants to output a string, you can do this by adding the OutputFormatter.h to your BOF and call the setOutputData() function. After finishing the BOF, the main program will call getOutputData() and get the output string.

## SOCK5 proxy
The proxy server BOF that bounces your connection through the implant to the internal network of the implant. The protocol used in the BOF is SOCK version 5 without any authentication.

## Network mapper
The network mapper BOF scans the internal network in which the implants is running from. Using TCP SYN, TCP CONNECT and ICMP ping scan methods.