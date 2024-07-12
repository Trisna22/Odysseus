# Metamorphism in Odysseus linux
Example for creating metamorphic binaries for every compile time.

## Tecnniques used
Based on randomization the code gets different shapes every time it get compiled. 

## Theory
### 1. Code Encryption:
    Malicious code is encrypted, and a decryption routine is included in the body of the code. The decryption routine is polymorphic, generating a different decryption algorithm each time the malware runs.

### 2. Code Obfuscation:
    Obfuscation techniques involve modifying the source code to make it difficult for humans to understand while keeping its functionality intact. This can include renaming variables, functions, and adding meaningless or redundant code.

### 3. Register and Instruction Operand Swapping:
    The order of registers and operands in instructions is changed. This doesn't affect the functionality but alters the binary representation of the code.

### 4. Dynamic Code Generation:
    The polymorphic engine dynamically generates code at runtime, making it harder for signature-based detection systems to identify the malicious patterns.

### 5. Garbage Code Insertion:
    Meaningless or non-executable code, often referred to as "garbage code," is inserted into the malware. This code does not affect the functionality but adds noise, making it more challenging to analyze.

### 6. Control Flow Obfuscation:
    The order of execution is altered using techniques such as jump instruction insertion, loop unrolling, or adding unnecessary conditional statements.

### 7. API Hashing:
    Instead of using standard API calls directly, malware authors may hash the API function names and dynamically resolve them during runtime, making static analysis more difficult.

### 8. Metamorphic Engines:
    Metamorphic engines are sophisticated tools that automatically rewrite the entire code while preserving its functionality. These engines can produce an endless variety of code variations.

### 9. Polymorphic Shell Code:
    In the context of exploiting vulnerabilities, polymorphic shell code changes its appearance while keeping the payload intact, making it harder for intrusion detection systems to recognize known attack patterns.

### 10. Code Permutation:
    Entire blocks of code are rearranged or swapped, altering the sequence of instructions. This makes it difficult to recognize the code based on static 

### 11. Instruction Substitution:
    Instructions are replaced with equivalent instructions that achieve the same result but have different opcodes. This makes the code look different while

### 12. Opaque Predicates:
    Metamorphic code may include deceptive conditions (opaque predicates) that are never true or false but serve to confuse analysis tools attempting to understand the program's logic.

### 13. Data Transformation:
    The representation of data within the code is transformed dynamically. This could include changing the encoding of strings or modifying numerical constants.

### 14. Dynamic API Resolution:
    Similar to polymorphic code, metamorphic code may dynamically resolve API function names during runtime, making it harder for static analysis to identify malicious behavior.