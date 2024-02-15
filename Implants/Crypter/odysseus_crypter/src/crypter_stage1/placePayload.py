#!/usr/bin/env python3
# Author:       ramb0
# Project:      Part of memory_binary_crypter
import sys

sourceFile = open("./src/stub/Stage1.h", "r")
source = sourceFile.read()

print("Placing (encrypted) buffer in target source...")
with open("./config/encryptedPayload.bin", "rb") as f:
    buffer = f.read().hex() # HEX encode so that compiler can read the code.
    source = source.replace("{{PAYLOAD}}", buffer)

print("Placing key in target source...")
with open("./config/key.dat", "r") as f:
    key = f.read()
    source = source.replace("{{KEY}}", key)    
    
with open("./src/stub/Stage1_generated.h", "w") as w:
    w.write(source)

print("Stub generated...\n")