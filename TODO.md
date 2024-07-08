# TODO
Change log for every push

## Change log

### (before) 27 aug 2023
Added the implant downloader  

### 6 sept 2023
Page for generating scripts for implant binaries created, backend doesn't work yet.

### 16 feb 2024
ELF implant can now execute BOF retrieved from the server. Also the JobHunter feature has been added to create parallel execution and job management.

### 1 mar 2024
Bug fix of JOB_ID in multi-threading. Cleanup and output with string possible.

### 24 may 2024
Added frontend functionality (web shell) + output formatter in BOFs

### 12 jun 2024
SOCK5 proxy server

## Todo list
```
ToDo:  

+ Client
  - Add feature to make the generator have the payload in a downloadable script,
so that the implants won't download the implant directly.
  - Add implant sleep and jitter options.
  - 

+ Implant (linux)
  - Add crypter functions
  - Sleep/jitter features
  - Relocation to add for object loading with classes.
    * R_X86_64_PC32
      + SYMBOL
      + SHF_INFO_LINK
    * R_X86_64_PLT32
      + SYMBOL
    * R_X86_64_64
      + SYMBOL
      + const char* getType(uint32_t info) {

          uint32_t type = ELF_R_TYPE(info);
          switch (type) {
              case R_X86_64_NONE: return "R_X86_64_NONE";
              case R_X86_64_64: return "R_X86_64_64";
              case R_X86_64_PC32: return "R_X86_64_PC32";
              case R_X86_64_GOT32: return "R_X86_64_GOT32";
              case R_X86_64_PLT32: return "R_X86_64_PLT32";
              case R_X86_64_COPY: return "R_X86_64_COPY";
              case R_X86_64_GLOB_DAT: return "R_X86_64_GLOB_DAT";
              case R_X86_64_JUMP_SLOT: return "R_X86_64_JUMP_SLOT";
              case R_X86_64_RELATIVE: return "R_X86_64_RELATIVE";
          case R_X86_64_GOTPCREL: return "R_X86_64_GOTPCREL";
              case R_X86_64_32: return "R_X86_64_32";
              case R_X86_64_32S: return "R_X86_64_32S";
              case R_X86_64_16: return "R_X86_64_16";
              case R_X86_64_PC16: return "R_X86_64_PC16";
              case R_X86_64_8: return "R_X86_64_8";
              case R_X86_64_PC8: return "R_X86_64_PC8";
              // Add other relocation types as needed
              default: return "Unknown";
          }
      }

+ Implant (Windows)
  - Add code obfuscation
  - Anti sandbox/edr
  - CoughLoader
  - Different payload execution methods (add options to implant generation):
    *  CreateRemoteThread()
    * ...

+ Implant (XLL)
  - Write a XLL excel implant
  - https://learn.microsoft.com/en-us/office/client-developer/excel/creating-xlls

+ Implant (mal. lnk)
  - https://assume-breach.medium.com/home-grown-red-team-using-lnk-files-to-bypass-applocker-3fb1ecae291f

+ Server
  - Windows implant compiler

+ Beacon Object Files
  - Network mapper (ping/port scan)
  - AMSI bypass
  - Browser password stealer

= Look at the example (GIF)
https://github.com/tdragon6/Supershell
```

## Resources
https://www.outflank.nl/blog/2018/08/14/html-smuggling-explained/
https://www.outflank.nl/blog/2019/05/05/evil-clippy-ms-office-maldoc-assistant/
https://resources.infosecinstitute.com/topics/hacking/deep-dive-into-a-custom-malware-packer/
