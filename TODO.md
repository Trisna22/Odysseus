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

## Todo list
```
ToDo:  

+ Client
  - Add feature to make the generator have the payload in a downloadable script,
so that the implants won't download the implant directly.
  - Add implant sleep and jitter options.
  - 

+ Implant
  - Add crypter functions
  - Different payload execution methods (add options to implant generation):
    *  CreateRemoteThread()
    * ...
  - Sleep/jitter features

+ Server
  - Windows implant compiler

+ Beacon Object Files
  - AMSI bypass
  - Browser password stealer
```

## Resources
https://www.outflank.nl/blog/2018/08/14/html-smuggling-explained/
https://www.outflank.nl/blog/2019/05/05/evil-clippy-ms-office-maldoc-assistant/
https://resources.infosecinstitute.com/topics/hacking/deep-dive-into-a-custom-malware-packer/
