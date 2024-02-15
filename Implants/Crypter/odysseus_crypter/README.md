# Crypter
Our crypter hides the actual code from static code analysis.  

## Workflow
EXAMPLE => STAGE 1 -> STAGE 2 -> STUB  
The STUB will be launched on the victim.

### STUB
The binary that will act as the finished crypter and can be deployed.

### CRYPTER STAGE 1
Places a encrypted buffer (EXAMPLE executable) in the STUB's memory so it can be executed with anonFile() in memory. It makes use of execle() fileless. 

### CRYPTER STAGE 2
Encrypts the STAGE 1 functionality in a section where it can decrypt itself in memory(page) when launched. Makes use of section decryption and hides the STAGE 1 functionality. 

### EXAMPLE 
The examle target binary we want to hide, but launch.

## Usage
```
make
make run
```

## Todo:
```
- Encrypt/Decrypt payload
- Using seeds to change the file signature
```

## Resources
https://github.com/droberson/ELFcrypt  
https://github.com/oldboy21/CGPL  
https://0x00sec.org/t/binary-armory-wiki/25679/9  
https://github.com/0x00pf/0x00sec_code/tree/master/crypter  
